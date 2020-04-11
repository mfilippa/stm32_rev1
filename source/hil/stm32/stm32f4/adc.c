// -----------------------------------------------------------------------------
// adc.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include <stdbool.h>
#include "stm32f4xx.h"
#include "adc.h"
#include "error.h"

// defines
#define ADC_MAX_FAST_COUNT  4   // for each ADC(1-3)
#define ADC_MAX_SLOW_COUNT  8   // total ADC1

// -----------------------------------------------------------------------------
// adc configuration

// STM32 internal temperature sensor (channel 16)
// Temp[degC] = ((VSENSE - V25) / AVG_SLOPE) + 25
// V25 = 0.76V
// AVG_SLOPE = 0.0025V/C

// ADC sample time, options: 3, 15, 28, 56, 84, 112, 144, 480 cycles
#define ADC_SAMPLE_TIME        ADC_SampleTime_84Cycles

// ADC fast count
// ADC1 max of 4, ADC2 and ADC3 max of 3
// ADC1 count must be always greater than ADC2 and ADC3 count, add dummy 
// conversions if needed (see FAST SEQUENCE CONFIGURATION below for explanation)
// Set count to zero to disable conversion
#define ADC1_FAST_COUNT     4       // max is ADC_MAX_FAST_COUNT
#define ADC2_FAST_COUNT     3       // max is ADC_MAX_FAST_COUNT-1
#define ADC3_FAST_COUNT     3       // max is ADC_MAX_FAST_COUNT-1

// ADC channel sequence
// Keep array size - fill up with dummy channels if not sampling all
// ADC1 available channels: ADC_Channel_0..15
uint32_t adc1_fast_ch_seq[ADC_MAX_FAST_COUNT] = {
    ADC_Channel_0,
    ADC_Channel_1,
    ADC_Channel_2,
    ADC_Channel_3,
};
// ADC2 available channels: ADC_Channel_0..15
uint32_t adc2_fast_ch_seq[ADC_MAX_FAST_COUNT-1] = {
    ADC_Channel_4,
    ADC_Channel_5,
    ADC_Channel_6,
};
// ADC3 available channels: ADC_Channel_0..3, 10..13
uint32_t adc3_fast_ch_seq[ADC_MAX_FAST_COUNT-1] = {
    ADC_Channel_10,
    ADC_Channel_11,
    ADC_Channel_12,
};

// fast conversion pwm trigger - set to true to trigger by PWM
bool adc_fast_pwm_trigger = true;

// ADC slow count
// Set count to zero to disable conversion
#define ADC1_SLOW_COUNT     8       // max is ADC_MAX_SLOW_COUNT

// ADC channel sequence
// Keep array size - fill up with dummy channels if not sampling all
// ADC1 available channels: ADC_Channel_0..15
uint32_t adc1_slow_ch_seq[ADC_MAX_SLOW_COUNT] = {
    ADC_Channel_0,
    ADC_Channel_1,
    ADC_Channel_2,
    ADC_Channel_3,
    ADC_Channel_4,
    ADC_Channel_5,
    ADC_Channel_6,
    ADC_Channel_7,
};

// slow conversion pwm trigger - set to true to trigger by PWM
bool adc_slow_pwm_trigger = true;

// -----------------------------------------------------------------------------

// more defines
#define ADC_FAST_COUNT (ADC1_FAST_COUNT+ADC2_FAST_COUNT+ADC3_FAST_COUNT)

// hardware pin mapping
struct adc_pin_struct {
    GPIO_TypeDef * gpio;
    uint32_t pin;
} adc_map[16] = {
        { GPIOA, GPIO_Pin_0 }, // ADC_Channel_0  (ADC123)
        { GPIOA, GPIO_Pin_1 }, // ADC_Channel_1  (ADC123)
        { GPIOA, GPIO_Pin_2 }, // ADC_Channel_2  (ADC123)
        { GPIOA, GPIO_Pin_3 }, // ADC_Channel_3  (ADC123)
        { GPIOA, GPIO_Pin_4 }, // ADC_Channel_4  (ADC12 )
        { GPIOA, GPIO_Pin_5 }, // ADC_Channel_5  (ADC12 )
        { GPIOA, GPIO_Pin_6 }, // ADC_Channel_6  (ADC12 )
        { GPIOA, GPIO_Pin_7 }, // ADC_Channel_7  (ADC12 )
        { GPIOB, GPIO_Pin_0 }, // ADC_Channel_8  (ADC12 )
        { GPIOB, GPIO_Pin_1 }, // ADC_Channel_9  (ADC12 )
        { GPIOC, GPIO_Pin_0 }, // ADC_Channel_10 (ADC123)
        { GPIOC, GPIO_Pin_1 }, // ADC_Channel_11 (ADC123)
        { GPIOC, GPIO_Pin_2 }, // ADC_Channel_12 (ADC123)
        { GPIOC, GPIO_Pin_3 }, // ADC_Channel_13 (ADC123)
        { GPIOC, GPIO_Pin_4 }, // ADC_Channel_14 (ADC12 )
        { GPIOC, GPIO_Pin_5 }, // ADC_Channel_15 (ADC12 )
};

// module structure
struct adc_struct {
    // handlers
    void (*slow_handler)(void);
    void (*fast_handler)(void);
    // raw data storage
    uint32_t fast_raw[ADC_FAST_COUNT];
    uint32_t slow_raw[ADC_CH_COUNT];
} adc;

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
void adc_init(void (*slow_handler)(void), void (*fast_handler)(void)) {

    GPIO_InitTypeDef GPIO_InitStruct;
    DMA_InitTypeDef DMA_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    ADC_CommonInitTypeDef ADC_CommonInitStruct;
    uint32_t i;

    // store data
    adc.slow_handler = slow_handler;
    adc.fast_handler = fast_handler;

    // range checks
    if (ADC1_FAST_COUNT>ADC_MAX_FAST_COUNT) error_raise(ERROR_ADC_INIT);
    if (ADC1_FAST_COUNT!=0){
        if (ADC2_FAST_COUNT>=ADC1_FAST_COUNT) error_raise(ERROR_ADC_INIT);
        if (ADC3_FAST_COUNT>=ADC1_FAST_COUNT) error_raise(ERROR_ADC_INIT);
    }
    if (ADC1_SLOW_COUNT>ADC_MAX_SLOW_COUNT) error_raise(ERROR_ADC_INIT);
    for (i=0;i<ADC_MAX_FAST_COUNT;i++){
        if (adc1_fast_ch_seq[i] >15) error_raise(ERROR_ADC_INIT);
    }
    for (i=0;i<ADC_MAX_FAST_COUNT-1;i++){
        if (adc2_fast_ch_seq[i] >15) error_raise(ERROR_ADC_INIT);
    }
    for (i=0;i<ADC_MAX_FAST_COUNT-1;i++){
        if (adc3_fast_ch_seq[i] >15) error_raise(ERROR_ADC_INIT);
    }
    for (i=0;i<ADC_MAX_SLOW_COUNT;i++){
        if (adc1_slow_ch_seq[i] >15) error_raise(ERROR_ADC_INIT);
    }

    // enable all required peripheral clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    // enable temperature sensor
    ADC_TempSensorVrefintCmd(ENABLE);

    // FAST PIN CONFIG
    if (ADC1_FAST_COUNT != 0) {

        // init gpio ...
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        // ... ADC1
        for (i = 0; i < ADC1_FAST_COUNT; i++) {
            GPIO_InitStruct.GPIO_Pin = adc_map[adc1_fast_ch_seq[i]].pin;
            GPIO_Init(adc_map[adc1_fast_ch_seq[i]].gpio, &GPIO_InitStruct);
        }
        // ... ADC2
        for (i = 0; i < ADC2_FAST_COUNT; i++) {
            GPIO_InitStruct.GPIO_Pin = adc_map[adc2_fast_ch_seq[i]].pin;
            GPIO_Init(adc_map[adc2_fast_ch_seq[i]].gpio, &GPIO_InitStruct);
        }
        // ... ADC3
        for (i = 0; i < ADC3_FAST_COUNT; i++) {
            GPIO_InitStruct.GPIO_Pin = adc_map[adc3_fast_ch_seq[i]].pin;
            GPIO_Init(adc_map[adc3_fast_ch_seq[i]].gpio, &GPIO_InitStruct);
        }
    }
    // END OF FAST PIN CONFIG

    // SLOW PIN CONFIG
    if (ADC1_SLOW_COUNT > 0) {

        // init gpio ...
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        // ... ADC1
        for (i = 0; i < ADC1_SLOW_COUNT; i++) {
            GPIO_InitStruct.GPIO_Pin = adc_map[adc1_slow_ch_seq[i]].pin;
            GPIO_Init(adc_map[adc1_slow_ch_seq[i]].gpio, &GPIO_InitStruct);
        }
        // DMA2 Stream0 -> ADC1
        DMA_InitStruct.DMA_Channel = DMA_Channel_0;
        DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) 0x4001204C;
        DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) adc.slow_raw;
        DMA_InitStruct.DMA_BufferSize = ADC1_SLOW_COUNT;
        DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
        DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
        DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
        DMA_InitStruct.DMA_Priority = DMA_Priority_High;
        DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
        DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
        DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
        DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
        DMA_Init(DMA2_Stream0, &DMA_InitStruct);
        // enable interrupt
        DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);// interrupt on transfer complete
        // enable stream
        DMA_Cmd(DMA2_Stream0, ENABLE);
    }
    // END OF SLOW PIN CONFIG

    // COMMON ADC INIT
    ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInit(&ADC_CommonInitStruct);

    // ADC1-3
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;// do one set of conversions and stop
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;    // SCAN: convert 0-n channels
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
    ADC_InitStruct.ADC_NbrOfConversion = ADC1_SLOW_COUNT;
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
    // ADC1 slow trigger: PWM or SW
    if ((adc_slow_pwm_trigger) && (ADC1_SLOW_COUNT != 0)) {
        ADC_InitStruct.ADC_ExternalTrigConvEdge =
        ADC_ExternalTrigConvEdge_Rising;
    } else {
        ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    }
    ADC_Init(ADC1, &ADC_InitStruct);
    // ADC2 trigger: SW only
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_Init(ADC2, &ADC_InitStruct);
    // ADC3 trigger: SW only
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_Init(ADC3, &ADC_InitStruct);
    // END OF COMMON ADC INIT

    // SLOW SEQUENCE CONFIGURATION
    if (ADC1_SLOW_COUNT > 0) {
        // ADC1 sequence config
        for (i = 0; i < ADC1_SLOW_COUNT; i++) {
            ADC_RegularChannelConfig(ADC1, adc1_slow_ch_seq[i], i + 1,
            ADC_SAMPLE_TIME);
        }

        // ADC1 continue issuing dma requests
        ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

        // ADC1 enable dma
        ADC_DMACmd(ADC1, ENABLE);
    }
    // END OF SLOW SEQUENCE CONFIGURATION

    // FAST SEQUENCE CONFIGURATION
    if (ADC1_FAST_COUNT != 0) {
        // injected config
        // Add an extra conversion so that interrupt is guaranteed to
        // be generated after ADC2 and ADC3 are done with conversion
        ADC_InjectedSequencerLengthConfig(ADC1, ADC1_FAST_COUNT);
        for (i=0;i<ADC1_FAST_COUNT;i++){
            ADC_InjectedChannelConfig(ADC1, adc1_fast_ch_seq[i], i+1, ADC_SAMPLE_TIME);
        }
        ADC_InjectedSequencerLengthConfig(ADC2, ADC2_FAST_COUNT);
        for (i=0;i<ADC2_FAST_COUNT;i++){
            ADC_InjectedChannelConfig(ADC2, adc2_fast_ch_seq[i], i+1, ADC_SAMPLE_TIME);
        }
        ADC_InjectedSequencerLengthConfig(ADC3, ADC3_FAST_COUNT);
        for (i=0;i<ADC3_FAST_COUNT;i++){
            ADC_InjectedChannelConfig(ADC3, adc3_fast_ch_seq[i], i+1, ADC_SAMPLE_TIME);
        }

        // injected trigger
        if (adc_fast_pwm_trigger) {
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC1, ADC_ExternalTrigInjecConvEdge_Rising);
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC2, ADC_ExternalTrigInjecConvEdge_Rising);
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC3, ADC_ExternalTrigInjecConvEdge_Rising);
        } else {
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC1, ADC_ExternalTrigInjecConvEdge_None);
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC2, ADC_ExternalTrigInjecConvEdge_None);
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC3, ADC_ExternalTrigInjecConvEdge_None);
        }
        ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);
        ADC_ExternalTrigInjectedConvConfig(ADC2, ADC_ExternalTrigInjecConv_T1_TRGO);
        ADC_ExternalTrigInjectedConvConfig(ADC3, ADC_ExternalTrigInjecConv_T1_TRGO);

        // JADC interrupt
        ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
    }
    // END OF FAST SEQUENCE CONFIGURATION

    // enable interrupts
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    NVIC_EnableIRQ(ADC_IRQn);

    // start ADC
    if ((ADC1_FAST_COUNT != 0) || (ADC1_SLOW_COUNT != 0)) ADC_Cmd(ADC1, ENABLE);
    if (ADC2_FAST_COUNT != 0) ADC_Cmd(ADC2, ENABLE);
    if (ADC3_FAST_COUNT != 0) ADC_Cmd(ADC3, ENABLE);

}

// -----------------------------------------------------------------------------
// trigger slow
// -----------------------------------------------------------------------------
void adc_sw_trigger_slow(void) {
    ADC_SoftwareStartConv(ADC1);
}

// -----------------------------------------------------------------------------
// trigger fast
// -----------------------------------------------------------------------------
void adc_sw_trigger_fast(void) {
    ADC_SoftwareStartInjectedConv(ADC3);
    ADC_SoftwareStartInjectedConv(ADC2);
    ADC_SoftwareStartInjectedConv(ADC1);
}

// -----------------------------------------------------------------------------
// ADC read
// -----------------------------------------------------------------------------
uint32_t adc_read(adc_channel_t channel) {
    if (channel >= ADC_CH_COUNT)
        return 0;
    else
        return adc.slow_raw[channel];
}

// -----------------------------------------------------------------------------
// ADC fast handler
// -----------------------------------------------------------------------------
void adc_fast_handler(void){
    // call handler
    if (adc.fast_handler != 0) {
        adc.fast_handler();
    }
}

// -----------------------------------------------------------------------------
// ADC slow handler
// -----------------------------------------------------------------------------
void adc_slow_handler(void){
    // call handler
    if (adc.slow_handler != 0) {
        adc.slow_handler();
    }
}

// -----------------------------------------------------------------------------
// DMA end of transfer irq
// -----------------------------------------------------------------------------
void DMA2_Stream0_IRQHandler(void) {
    // clear flag
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
    // call handler
    adc_slow_handler();
}

// -----------------------------------------------------------------------------
// JADC EOC interrupt
// -----------------------------------------------------------------------------
void ADC_IRQHandler(void) {
    int i, j;
    // clear flag
    ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
    // store converted values
    j = 0;
    // ADC1
    for (i = 0; i < ADC1_FAST_COUNT; i++) {
        adc.fast_raw[j] = ADC_GetInjectedConversionValue(ADC1,
        ADC_InjectedChannel_1 + 4 * i);
        j++;
    }
    // ADC2
    for (i = 0; i < ADC2_FAST_COUNT; i++) {
        adc.fast_raw[j] = ADC_GetInjectedConversionValue(ADC2,
        ADC_InjectedChannel_1 + 4 * i);
        j++;
    }
    // ADC3
    for (i = 0; i < ADC3_FAST_COUNT; i++) {
        adc.fast_raw[j] = ADC_GetInjectedConversionValue(ADC3,
        ADC_InjectedChannel_1 + 4 * i);
        j++;
    }
    // call handler
    adc_fast_handler();
}

