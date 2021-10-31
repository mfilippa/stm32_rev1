// -----------------------------------------------------------------------------
// adc.c - MPF 10/2021
// -----------------------------------------------------------------------------

// includes
#include <stdbool.h>
#include "stm32f4xx.h"
#include "adc.h"
#include "error.h"

// -----------------------------------------------------------------------------
// ADC Configuration
//
// Two types of ADCs:
// - Fast (injected): takes priority over regular ADCs, results to registers
// - Slow (regular): regular ADC sampling, results to buffer via DMA
// Both can be triggered by either PWM or SW trigger

// comment to disable conversion
#define ADC_ENABLE_FAST
#define ADC_ENABLE_SLOW

// STM32 internal temperature sensor (channel 16)
// Temp[degC] = ((VSENSE - V25) / AVG_SLOPE) + 25
// V25 = 0.76V
// AVG_SLOPE = 0.0025V/C

// ADC sample time, options: 3, 15, 28, 56, 84, 112, 144, 480 cycles
#define ADC_SAMPLE_TIME        ADC_SampleTime_84Cycles

// ADC fast configuration ------------------------------------------------------
// ADC1 count must be always greater than ADC2 and ADC3 count, add dummy 
// conversions if needed (see FAST SEQUENCE CONFIGURATION below for explanation)
// Set ADC2/3 count to zero to disable conversion
#define ADC1_FAST_COUNT     4      // range: 1..4
#define ADC2_FAST_COUNT     3       // range: 0..3
#define ADC3_FAST_COUNT     3       // range: 0..3
#define ADC_FAST_CH_COUNT (ADC1_FAST_COUNT+ADC2_FAST_COUNT+ADC3_FAST_COUNT)

// ADC channel sequence
// ADC1 available channels: ADC_Channel_0..15
// ADC2 available channels: ADC_Channel_0..15
// ADC3 available channels: ADC_Channel_0..3, 10..13
// comment unused channels
uint32_t adc_fast_ch_seq[ADC_FAST_CH_COUNT] = {
    // ADC1
    ADC_Channel_0,
    ADC_Channel_1,
    ADC_Channel_2,
    ADC_Channel_3,
    // ADC2
    ADC_Channel_4,
    ADC_Channel_5,
    ADC_Channel_6,
    // ADC3
    ADC_Channel_10,
    ADC_Channel_11,
    ADC_Channel_12,
};

// fast conversion triggered by PWM - comment to disable
#define ADC_FAST_PWM_TRIGGER

// ADC slow configuration ------------------------------------------------------
// Set count to zero to disable conversion
#define ADC_SLOW_CH_COUNT     8       // range: 1..8

// ADC channel sequence
// ADC1 available channels: ADC_Channel_0..15
// comment unused channels
uint32_t adc_slow_ch_seq[ADC_SLOW_CH_COUNT] = {
    ADC_Channel_0,
    ADC_Channel_1,
    ADC_Channel_2,
    ADC_Channel_3,
    ADC_Channel_4,
    ADC_Channel_5,
    ADC_Channel_6,
    ADC_Channel_16,
};

// slow conversion triggered by pwm - comment to disable
// PWM frequency divider in pwm.c module
#define ADC_SLOW_PWM_TRIGGER

// End of ADC configuration
// -----------------------------------------------------------------------------

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
    uint32_t fast_raw[ADC_FAST_CH_COUNT];
    uint32_t slow_raw[ADC_SLOW_CH_COUNT];
} adc;

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
void adc_init(void (*fast_handler)(void), void (*slow_handler)(void)) {

    ADC_InitTypeDef ADC_InitStruct;
    ADC_CommonInitTypeDef ADC_CommonInitStruct;
    #if defined(ADC_ENABLE_SLOW) || defined(ADC_ENABLE_FAST)
        GPIO_InitTypeDef GPIO_InitStruct;
    #endif
    #ifdef ADC_ENABLE_SLOW
        DMA_InitTypeDef DMA_InitStruct;
    #endif
    uint32_t i;

    // store data
    adc.slow_handler = slow_handler;
    adc.fast_handler = fast_handler;

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

    #ifdef ADC_ENABLE_FAST
        // FAST PIN CONFIG
        // init gpio ...
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        // ... ADC1
        for (i = 0; i < ADC1_FAST_COUNT; i++) {
            GPIO_InitStruct.GPIO_Pin = adc_map[adc_fast_ch_seq[i]].pin;
            GPIO_Init(adc_map[adc_fast_ch_seq[i]].gpio, &GPIO_InitStruct);
        }
        // ... ADC2
        #if ADC2_FAST_COUNT != 0
            for (i = 0; i < ADC2_FAST_COUNT; i++) {
                GPIO_InitStruct.GPIO_Pin = adc_map[adc_fast_ch_seq[ADC1_FAST_COUNT+i]].pin;
                GPIO_Init(adc_map[adc_fast_ch_seq[ADC1_FAST_COUNT+i]].gpio, &GPIO_InitStruct);
            }
        #endif
        // ... ADC3
        #if ADC3_FAST_COUNT != 0
            for (i = 0; i < ADC3_FAST_COUNT; i++) {
                GPIO_InitStruct.GPIO_Pin = adc_map[adc_fast_ch_seq[ADC1_FAST_COUNT+ADC2_FAST_COUNT+i]].pin;
                GPIO_Init(adc_map[adc_fast_ch_seq[ADC1_FAST_COUNT+ADC2_FAST_COUNT+i]].gpio, &GPIO_InitStruct);
            }
        #endif
    // END OF FAST PIN CONFIG
    #endif

    #ifdef ADC_ENABLE_SLOW
        // SLOW PIN CONFIG
        // init gpio ...
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        // ... ADC1
        for (i = 0; i < ADC_SLOW_CH_COUNT; i++) {
            GPIO_InitStruct.GPIO_Pin = adc_map[adc_slow_ch_seq[i]].pin;
            GPIO_Init(adc_map[adc_slow_ch_seq[i]].gpio, &GPIO_InitStruct);
        }
        // DMA2 Stream0 -> ADC1
        DMA_InitStruct.DMA_Channel = DMA_Channel_0;
        DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) 0x4001204C;
        DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) adc.slow_raw;
        DMA_InitStruct.DMA_BufferSize = ADC_SLOW_CH_COUNT;
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
        // END OF SLOW PIN CONFIG
    #endif

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
    ADC_InitStruct.ADC_NbrOfConversion = ADC_SLOW_CH_COUNT;
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
    // ADC1 slow trigger: PWM or SW
    #if defined(ADC_SLOW_PWM_TRIGGER)
        ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    #else
        ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    #endif
    ADC_Init(ADC1, &ADC_InitStruct);
    // ADC2 trigger: SW only
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_Init(ADC2, &ADC_InitStruct);
    // ADC3 trigger: SW only
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_Init(ADC3, &ADC_InitStruct);
    // END OF COMMON ADC INIT

    // SLOW SEQUENCE CONFIGURATION
    // ADC1 sequence config
    for (i = 0; i < ADC_SLOW_CH_COUNT; i++) {
        ADC_RegularChannelConfig(ADC1, adc_slow_ch_seq[i], i + 1,
            ADC_SAMPLE_TIME);
    }

    // ADC1 continue issuing dma requests
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    // ADC1 enable dma
    ADC_DMACmd(ADC1, ENABLE);
    // END OF SLOW SEQUENCE CONFIGURATION

    // FAST SEQUENCE CONFIGURATION

    // injected config
    // Add an extra conversion so that interrupt is guaranteed to
    // be generated after ADC2 and ADC3 are done with conversion
    ADC_InjectedSequencerLengthConfig(ADC1, ADC1_FAST_COUNT);
    for (i=0;i<ADC1_FAST_COUNT;i++){
        ADC_InjectedChannelConfig(ADC1, adc_fast_ch_seq[i], i+1, ADC_SAMPLE_TIME);
    }
    #if (ADC2_FAST_COUNT!=0)
        ADC_InjectedSequencerLengthConfig(ADC2, ADC2_FAST_COUNT);
        for (i=0;i<ADC2_FAST_COUNT;i++){
            ADC_InjectedChannelConfig(ADC2, adc_fast_ch_seq[ADC1_FAST_COUNT+i], i+1, ADC_SAMPLE_TIME);
        }
    #endif
    #if (ADC3_FAST_COUNT!=0)
        ADC_InjectedSequencerLengthConfig(ADC3, ADC3_FAST_COUNT);
        for (i=0;i<ADC3_FAST_COUNT;i++){
            ADC_InjectedChannelConfig(ADC3, adc_fast_ch_seq[ADC1_FAST_COUNT+ADC2_FAST_COUNT+i], i+1, ADC_SAMPLE_TIME);
        }
    #endif

    // injected trigger
    #ifdef ADC_ENABLE_FAST
        #ifdef ADC_FAST_PWM_TRIGGER
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC1, ADC_ExternalTrigInjecConvEdge_Rising);
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC2, ADC_ExternalTrigInjecConvEdge_Rising);
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC3, ADC_ExternalTrigInjecConvEdge_Rising);
        #else
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC1, ADC_ExternalTrigInjecConvEdge_None);
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC2, ADC_ExternalTrigInjecConvEdge_None);
            ADC_ExternalTrigInjectedConvEdgeConfig(ADC3, ADC_ExternalTrigInjecConvEdge_None);
        #endif
    #endif
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);
    ADC_ExternalTrigInjectedConvConfig(ADC2, ADC_ExternalTrigInjecConv_T1_TRGO);
    ADC_ExternalTrigInjectedConvConfig(ADC3, ADC_ExternalTrigInjecConv_T1_TRGO);

    // JADC interrupt
    #ifdef ADC_ENABLE_FAST
        ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
    #endif

    // END OF FAST SEQUENCE CONFIGURATION

    // enable interrupts
    #ifdef ADC_ENABLE_SLOW
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    #endif
    #ifdef ADC_ENABLE_FAST
    NVIC_EnableIRQ(ADC_IRQn);
    #endif

    // start ADC
    #if defined(ADC_ENABLE_FAST) || defined(ADC_ENABLE_SLOW)
        ADC_Cmd(ADC1, ENABLE);
    #endif
    #if (ADC2_FAST_COUNT != 0) 
        ADC_Cmd(ADC2, ENABLE);
    #endif
    #if (ADC3_FAST_COUNT != 0) 
        ADC_Cmd(ADC3, ENABLE);
    #endif

}

// -----------------------------------------------------------------------------
// trigger slow
// -----------------------------------------------------------------------------
void adc_sw_trigger_slow(void) {
    #ifdef ADC_ENABLE_SLOW
        ADC_SoftwareStartConv(ADC1);
    #endif
}

// -----------------------------------------------------------------------------
// trigger fast
// -----------------------------------------------------------------------------
void adc_sw_trigger_fast(void) {
    #ifdef ADC_ENABLE_FAST
        #if ADC3_FAST_COUNT != 0
            ADC_SoftwareStartInjectedConv(ADC3);
        #endif
        #if ADC2_FAST_COUNT != 0
            ADC_SoftwareStartInjectedConv(ADC2);
        #endif
        ADC_SoftwareStartInjectedConv(ADC1);
    #endif
}

// -----------------------------------------------------------------------------
// ADC read fast
// -----------------------------------------------------------------------------
uint32_t adc_read_fast(adc_fast_channel_t channel) {
    // no error checking for speed
    return adc.fast_raw[channel];
}

// -----------------------------------------------------------------------------
// ADC read slow
// -----------------------------------------------------------------------------
uint32_t adc_read_slow(adc_slow_channel_t channel) {
    // no error checking for speed
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
    #if ADC2_FAST_COUNT != 0
    for (i = 0; i < ADC2_FAST_COUNT; i++) {
        adc.fast_raw[j] = ADC_GetInjectedConversionValue(ADC2,
        ADC_InjectedChannel_1 + 4 * i);
        j++;
    }
    #endif
    // ADC3
    #if ADC3_FAST_COUNT != 0
    for (i = 0; i < ADC3_FAST_COUNT; i++) {
        adc.fast_raw[j] = ADC_GetInjectedConversionValue(ADC3,
        ADC_InjectedChannel_1 + 4 * i);
        j++;
    }
    #endif
    // call handler
    adc_fast_handler();
}

