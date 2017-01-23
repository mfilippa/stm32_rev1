// -----------------------------------------------------------------------------
// adc.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "stm32f4xx.h"
#include "hal/adc.h"

// triggers: comment for SW trigger
//#define ADC_SLOW_TRIGGER_PWM
//#define ADC_FAST_TRIGGER_PWM

// ADC sample time
#define ADC_SAMPLE_TIME		ADC_SampleTime_15Cycles

// adc pin structure
typedef struct io_pin_struct {
	GPIO_TypeDef * gpio;
	uint32_t pin;
	uint8_t channel;
} adc_pin_t;

// module structure
struct adc_struct {
	adc_pin_t pin_fast[ADC_FAST_COUNT];
	adc_pin_t pin_slow[ADC_SLOW_COUNT];
	void (*slow_handler)(void);
	void (*fast_handler)(void);
	uint32_t raw_fast[ADC_FAST_COUNT];
	uint32_t raw_slow[ADC_SLOW_COUNT];
} adc;

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t adc_init(void (*slow_handler)(void), void (*fast_handler)(void)) {

	GPIO_InitTypeDef GPIO_InitStruct;
	DMA_InitTypeDef DMA_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	uint32_t i;

	// store handlers
	adc.slow_handler = slow_handler;
	adc.fast_handler = fast_handler;

	// enable all required peripheral clocks here -----------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	// ------------------------------------------------------

	// configure channels here ------------------------------
	// ADC_FAST_CH1
	adc.pin_fast[ADC_FAST_CH1].gpio = GPIOA;
	adc.pin_fast[ADC_FAST_CH1].pin = GPIO_Pin_0;
	adc.pin_fast[ADC_FAST_CH1].channel = ADC_Channel_0;
	// ADC_FAST_CH2
	adc.pin_fast[ADC_FAST_CH2].gpio = GPIOA;
	adc.pin_fast[ADC_FAST_CH2].pin = GPIO_Pin_1;
	adc.pin_fast[ADC_FAST_CH2].channel = ADC_Channel_1;
	// ADC_FAST_CH3
	adc.pin_fast[ADC_FAST_CH3].gpio = GPIOA;
	adc.pin_fast[ADC_FAST_CH3].pin = GPIO_Pin_2;
	adc.pin_fast[ADC_FAST_CH3].channel = ADC_Channel_2;
	// ADC_FAST_CH4
	adc.pin_fast[ADC_FAST_CH4].gpio = GPIOA;
	adc.pin_fast[ADC_FAST_CH4].pin = GPIO_Pin_3;
	adc.pin_fast[ADC_FAST_CH4].channel = ADC_Channel_3;
	// ADC_FAST_CH5
	adc.pin_fast[ADC_FAST_CH5].gpio = GPIOC;
	adc.pin_fast[ADC_FAST_CH5].pin = GPIO_Pin_0;
	adc.pin_fast[ADC_FAST_CH5].channel = ADC_Channel_10;
	// ADC_FAST_CH6
	adc.pin_fast[ADC_FAST_CH6].gpio = GPIOC;
	adc.pin_fast[ADC_FAST_CH6].pin = GPIO_Pin_1;
	adc.pin_fast[ADC_FAST_CH6].channel = ADC_Channel_11;
	// ADC_FAST_CH7
	adc.pin_fast[ADC_FAST_CH7].gpio = GPIOC;
	adc.pin_fast[ADC_FAST_CH7].pin = GPIO_Pin_2;
	adc.pin_fast[ADC_FAST_CH7].channel = ADC_Channel_12;
	// ADC_FAST_CH8
	adc.pin_fast[ADC_FAST_CH8].gpio = GPIOC;
	adc.pin_fast[ADC_FAST_CH8].pin = GPIO_Pin_3;
	adc.pin_fast[ADC_FAST_CH8].channel = ADC_Channel_13;
	// ADC_SLOW_CH1
	adc.pin_slow[ADC_SLOW_CH1].gpio = GPIOA;
	adc.pin_slow[ADC_SLOW_CH1].pin = GPIO_Pin_6;
	adc.pin_slow[ADC_SLOW_CH1].channel = ADC_Channel_6;
	// ADC_SLOW_CH2
	adc.pin_slow[ADC_SLOW_CH2].gpio = GPIOA;
	adc.pin_slow[ADC_SLOW_CH2].pin = GPIO_Pin_7;
	adc.pin_slow[ADC_SLOW_CH2].channel = ADC_Channel_7;
	// ADC_SLOW_CH3
	adc.pin_slow[ADC_SLOW_CH3].gpio = GPIOB;
	adc.pin_slow[ADC_SLOW_CH3].pin = GPIO_Pin_0;
	adc.pin_slow[ADC_SLOW_CH3].channel = ADC_Channel_8;
	// ADC_SLOW_CH4
	adc.pin_slow[ADC_SLOW_CH4].gpio = GPIOB;
	adc.pin_slow[ADC_SLOW_CH4].pin = GPIO_Pin_1;
	adc.pin_slow[ADC_SLOW_CH4].channel = ADC_Channel_9;
//	// ADC_SLOW_CH5
//	adc.pin_slow[ADC_SLOW_CH5].gpio = GPIOC;
//	adc.pin_slow[ADC_SLOW_CH5].pin = GPIO_Pin_0;
//	adc.pin_slow[ADC_SLOW_CH5].channel = ADC_Channel_10;
//	// ADC_SLOW_CH6
//	adc.pin_slow[ADC_SLOW_CH6].gpio = GPIOC;
//	adc.pin_slow[ADC_SLOW_CH6].pin = GPIO_Pin_0;
//	adc.pin_slow[ADC_SLOW_CH6].channel = ADC_Channel_10;
//	// ADC_SLOW_CH7
//	adc.pin_slow[ADC_SLOW_CH7].gpio = GPIOC;
//	adc.pin_slow[ADC_SLOW_CH7].pin = GPIO_Pin_0;
//	adc.pin_slow[ADC_SLOW_CH7].channel = ADC_Channel_10;
//	// ADC_SLOW_CH8
//	adc.pin_slow[ADC_SLOW_CH8].gpio = GPIOC;
//	adc.pin_slow[ADC_SLOW_CH8].pin = GPIO_Pin_0;
//	adc.pin_slow[ADC_SLOW_CH8].channel = ADC_Channel_10;
	// ------------------------------------------------------

	// init gpio
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	for (i = 0; i < ADC_FAST_COUNT; i++) {
		if (adc.pin_fast[i].gpio != 0) {
			GPIO_InitStruct.GPIO_Pin = adc.pin_fast[i].pin;
			GPIO_Init(adc.pin_fast[i].gpio, &GPIO_InitStruct);
		}
	}
	for (i = 0; i < ADC_SLOW_COUNT; i++) {
		if (adc.pin_slow[i].gpio != 0) {
			GPIO_InitStruct.GPIO_Pin = adc.pin_slow[i].pin;
			GPIO_Init(adc.pin_slow[i].gpio, &GPIO_InitStruct);
		}
	}

	// DMA ---------------------------------------------------------------------
	// DMA2 Stream0 -> ADC1
	DMA_InitStruct.DMA_Channel = DMA_Channel_0;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) 0x4001204C;
	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) adc.raw_slow;
	DMA_InitStruct.DMA_BufferSize = ADC_SLOW_COUNT;
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

	// ADC ---------------------------------------------------------------------
	// common init
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInit(&ADC_CommonInitStruct);

	// ADC1-3 init
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;// do one set of conversions and stop
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;	// SCAN: convert 0-n channels
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
	ADC_InitStruct.ADC_NbrOfConversion = ADC_SLOW_COUNT;
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	// ADC1 trigger: PWM or SW
#if defined ADC_SLOW_TRIGGER_PWM
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

	// ADC1 sequence config
	for (i = 0; i < ADC_SLOW_COUNT; i++) {
		ADC_RegularChannelConfig(ADC1, adc.pin_slow[i].channel, i + 1,
		ADC_SAMPLE_TIME);
	}

	// ADC1 continue issuing dma requests
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	// ADC1 enable dma
	ADC_DMACmd(ADC1, ENABLE);

	// injected config
	// Add an extra conversion so that interrupt is guaranteed to
	// be generated after ADC2 and ADC3 are done with conversion
	ADC_InjectedSequencerLengthConfig(ADC1, 4);
	ADC_InjectedChannelConfig(ADC1, adc.pin_fast[ADC_FAST_CH1].channel, 1,
	ADC_SAMPLE_TIME);
	ADC_InjectedChannelConfig(ADC1, adc.pin_fast[ADC_FAST_CH2].channel, 2,
	ADC_SAMPLE_TIME);
	ADC_InjectedChannelConfig(ADC1, adc.pin_fast[ADC_FAST_CH3].channel, 3,
	ADC_SAMPLE_TIME);
	ADC_InjectedChannelConfig(ADC1, adc.pin_fast[ADC_FAST_CH4].channel, 4,
	ADC_SAMPLE_TIME);
	ADC_InjectedSequencerLengthConfig(ADC2, 2);
	ADC_InjectedChannelConfig(ADC2, adc.pin_fast[ADC_FAST_CH5].channel, 1,
	ADC_SAMPLE_TIME);
	ADC_InjectedChannelConfig(ADC2, adc.pin_fast[ADC_FAST_CH6].channel, 2,
	ADC_SAMPLE_TIME);
	ADC_InjectedSequencerLengthConfig(ADC3, 2);
	ADC_InjectedChannelConfig(ADC3, adc.pin_fast[ADC_FAST_CH7].channel, 1,
	ADC_SAMPLE_TIME);
	ADC_InjectedChannelConfig(ADC3, adc.pin_fast[ADC_FAST_CH8].channel, 2,
	ADC_SAMPLE_TIME);

	// injected trigger
#if defined ADC_FAST_TRIGGER_PWM
	ADC_ExternalTrigInjectedConvEdgeConfig(ADC1, ADC_ExternalTrigInjecConvEdge_Rising);
	ADC_ExternalTrigInjectedConvEdgeConfig(ADC2, ADC_ExternalTrigInjecConvEdge_Rising);
	ADC_ExternalTrigInjectedConvEdgeConfig(ADC3, ADC_ExternalTrigInjecConvEdge_Rising);
#else
	ADC_ExternalTrigInjectedConvEdgeConfig(ADC1,
	ADC_ExternalTrigInjecConvEdge_None);
	ADC_ExternalTrigInjectedConvEdgeConfig(ADC2,
	ADC_ExternalTrigInjecConvEdge_None);
	ADC_ExternalTrigInjectedConvEdgeConfig(ADC3,
	ADC_ExternalTrigInjecConvEdge_None);
#endif
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);
	ADC_ExternalTrigInjectedConvConfig(ADC2, ADC_ExternalTrigInjecConv_T1_TRGO);
	ADC_ExternalTrigInjectedConvConfig(ADC3, ADC_ExternalTrigInjecConv_T1_TRGO);

	// JADC interrupt
	ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);

	// enable interrupts
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);

	// start ADC
	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);
	ADC_Cmd(ADC3, ENABLE);

	// success
	return 0;
}

// -----------------------------------------------------------------------------
// trigger slow
// -----------------------------------------------------------------------------
void adc_trigger_slow(void) {
	ADC_SoftwareStartConv(ADC1);
}

// -----------------------------------------------------------------------------
// trigger fast
// -----------------------------------------------------------------------------
void adc_trigger_fast(void) {
	ADC_SoftwareStartInjectedConv(ADC3);
	ADC_SoftwareStartInjectedConv(ADC2);
	ADC_SoftwareStartInjectedConv(ADC1);
}

// -----------------------------------------------------------------------------
// read slow
// -----------------------------------------------------------------------------
uint32_t adc_read_slow(adc_slow_t channel) {
	if (channel >= ADC_SLOW_COUNT)
		return 0;
	else
		return adc.raw_slow[channel];
}

// -----------------------------------------------------------------------------
// read fast
// -----------------------------------------------------------------------------
uint32_t adc_read_fast(adc_fast_t channel) {
	if (channel >= ADC_FAST_COUNT)
		return 0;
	else
		return adc.raw_fast[channel];
}

// -----------------------------------------------------------------------------
// DMA end of transfer irq
// -----------------------------------------------------------------------------
void DMA2_Stream0_IRQHandler(void) {
	// clear flag
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	// call handler
	if (adc.slow_handler != 0) {
		adc.slow_handler();
	}
}

// -----------------------------------------------------------------------------
// JADC EOC interrupt
// -----------------------------------------------------------------------------
void ADC_IRQHandler(void) {
	// clear flag
	ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
	// store converted values
	adc.raw_fast[ADC_FAST_CH1] = ADC_GetInjectedConversionValue(ADC1,
	ADC_InjectedChannel_1);
	adc.raw_fast[ADC_FAST_CH2] = ADC_GetInjectedConversionValue(ADC1,
	ADC_InjectedChannel_2);
	adc.raw_fast[ADC_FAST_CH3] = ADC_GetInjectedConversionValue(ADC1,
	ADC_InjectedChannel_3);
	adc.raw_fast[ADC_FAST_CH4] = ADC_GetInjectedConversionValue(ADC1,
	ADC_InjectedChannel_4);
	adc.raw_fast[ADC_FAST_CH5] = ADC_GetInjectedConversionValue(ADC2,
	ADC_InjectedChannel_1);
	adc.raw_fast[ADC_FAST_CH6] = ADC_GetInjectedConversionValue(ADC2,
	ADC_InjectedChannel_2);
	adc.raw_fast[ADC_FAST_CH7] = ADC_GetInjectedConversionValue(ADC3,
	ADC_InjectedChannel_1);
	adc.raw_fast[ADC_FAST_CH8] = ADC_GetInjectedConversionValue(ADC3,
	ADC_InjectedChannel_2);
	// call handler
	if (adc.fast_handler != 0) {
		adc.fast_handler();
	}
}

