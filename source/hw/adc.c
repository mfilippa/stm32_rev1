// -----------------------------------------------------------------------------
// adc.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "stm32f4xx.h"
#include "hal/adc.h"

// -----------------------------------------------------------------------------
// configuration
// -----------------------------------------------------------------------------

// ADC fast counts
#define ADC1_FAST_COUNT		4	// zero to disable, 1 to 4
#define ADC2_FAST_COUNT		3	// must be < ADC1_FAST_COUNT, zero to disable
#define ADC3_FAST_COUNT		3	// must be < ADC1_FAST_COUNT, zero to disable
#define ADC_FAST_COUNT	(ADC1_FAST_COUNT+ADC2_FAST_COUNT+ADC3_FAST_COUNT)

// ADC1 fast sequence (ADC channel n)
#define ADC1_FAST_CH1	0
#define ADC1_FAST_CH2	1
#define ADC1_FAST_CH3	2
#define ADC1_FAST_CH4	3

// ADC2 fast sequence (ADC channel n)
#define ADC2_FAST_CH1	4
#define ADC2_FAST_CH2	5
#define ADC2_FAST_CH3	6

// ADC3 fast sequence (ADC channel n)
#define ADC3_FAST_CH1	7
#define ADC3_FAST_CH2	8
#define ADC3_FAST_CH3	9

// ADC FAST TRIGGER - 1 for PWM, 0 for SW
#define ADC_FAST_PWM_TRIGGER   0

// ADC slow count
#define ADC_SLOW_COUNT	8	// zero to disable, 1 to 8

// ADC slow sequence
#define ADC_SLOW_CH1	0
#define ADC_SLOW_CH2	1
#define ADC_SLOW_CH3	2
#define ADC_SLOW_CH4	3
#define ADC_SLOW_CH5	4
#define ADC_SLOW_CH6	5
#define ADC_SLOW_CH7	6
#define ADC_SLOW_CH8	7

// ADC SLOW TRIGGER - 1 for PWM, 0 for SW
#define ADC_SLOW_PWM_TRIGGER  0

// -----------------------------------------------------------------------------
// end of configuration
// -----------------------------------------------------------------------------

// defines
#define ADC_MAX_FAST_COUNT	4	// for each ADC(1-3)
#define ADC_MAX_SLOW_COUNT	8	// total ADC1
#define ADC_NR_CONVERTERS	3	// ADC1-3

// injected check
#if (ADC1_FAST_COUNT!=0)
#if ((ADC2_FAST_COUNT>=ADC1_FAST_COUNT)||(ADC3_FAST_COUNT>=ADC1_FAST_COUNT))
#error "ADC1_FAST_COUNTmust be > than ADC2_FAST_COUNT and ADC3_FAST_COUNT"
#endif
#endif

// ADC configuration table
typedef struct adc_config_struct {
	uint32_t fast_channel[ADC_NR_CONVERTERS][ADC_MAX_FAST_COUNT];
	uint32_t fast_count[ADC_NR_CONVERTERS];
	uint32_t fast_pwm_trigger;
	uint32_t slow_channel[ADC_MAX_SLOW_COUNT];
	uint32_t slow_count;
	uint32_t slow_pwm_trigger;
} adc_config_t;

// example config:
//adc_config_t adc_config = {
//	{ { 0, 1, 2, 3 },	// ADC1 fast sequence
//	{ 4, 5, 6, 0 },		// ADC2 fast sequence
//	{ 7, 8, 9, 0 } },	// ADC3 fast sequence
//	{ 4, 3, 3 },		// ADC fast lengths (ADC1>ADC2, ADC1>ADC3, zero to disable)
//	1,					// 1 for PWM trigger, 0 for SW trigger
//	{ 0, 1, 2, 3, 4, 5, 0, 0 },	// ADC1 slow sequence
//	6, 					// ADC slow length, zero to disable
//	0, 					// 1 for PWM trigger, 0 for SW trigger
//};

// ADC configuration
adc_config_t adc_config = {
// ADC1 fast sequence
		{ { ADC1_FAST_CH1, ADC1_FAST_CH2, ADC1_FAST_CH3,
		ADC1_FAST_CH4 },
		// ADC2 fast sequence
		{ ADC2_FAST_CH1, ADC2_FAST_CH2, ADC2_FAST_CH3, 0 },
		// ADC3 fast sequence
		{ ADC3_FAST_CH1, ADC3_FAST_CH2, ADC3_FAST_CH3, 0 } },
		// ADC fast lengths (ADC1>ADC2, ADC1>ADC3, zero to disable)
		{ ADC1_FAST_COUNT, ADC2_FAST_COUNT, ADC3_FAST_COUNT },
		// 1 for PWM trigger, 0 for SW trigger
		ADC_FAST_PWM_TRIGGER,
		// ADC1 slow sequence
		{ ADC_SLOW_CH1, ADC_SLOW_CH2, ADC_SLOW_CH3, ADC_SLOW_CH4, ADC_SLOW_CH5,
		ADC_SLOW_CH6, ADC_SLOW_CH7, ADC_SLOW_CH8 },
		// ADC slow length, zero to disable
		ADC_SLOW_COUNT,
		// 1 for PWM trigger, 0 for SW trigger
		ADC_SLOW_PWM_TRIGGER,
};

// hardware pin mapping
struct adc_pin_struct {
	GPIO_TypeDef * gpio;
	uint32_t pin;
} adc_map[16] = { { GPIOA, GPIO_Pin_0 }, // ADC_Channel_0  (ADC123)
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

// ADC sample time
#define ADC_SAMPLE_TIME		ADC_SampleTime_15Cycles

// module structure
struct adc_struct {
	uint32_t raw[ADC_FAST_COUNT+ADC_SLOW_COUNT]; // maximize storage
	void (*slow_handler)(void);
	void (*fast_handler)(void);
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

	// FAST PIN CONFIG
	if (adc_config.fast_count[0] != 0) {

		// init gpio ...
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		// ... ADC1
		for (i = 0; i < adc_config.fast_count[0]; i++) {
			GPIO_InitStruct.GPIO_Pin = adc_map[adc_config.fast_channel[0][i]].pin;
			GPIO_Init(adc_map[adc_config.fast_channel[0][i]].gpio,
					&GPIO_InitStruct);
		}
		// ... ADC2
		for (i = 0; i < adc_config.fast_count[1]; i++) {
			GPIO_InitStruct.GPIO_Pin = adc_map[adc_config.fast_channel[1][i]].pin;
			GPIO_Init(adc_map[adc_config.fast_channel[1][i]].gpio,
					&GPIO_InitStruct);
		}
		// ... ADC3
		for (i = 0; i < adc_config.fast_count[2]; i++) {
			GPIO_InitStruct.GPIO_Pin = adc_map[adc_config.fast_channel[2][i]].pin;
			GPIO_Init(adc_map[adc_config.fast_channel[2][i]].gpio,
					&GPIO_InitStruct);
		}
	}
	// END OF FAST PIN CONFIG

	// SLOW PIN CONFIG
	if (adc_config.slow_count > 0) {

		// init gpio ...
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		// ... ADC1
		for (i = 0; i < adc_config.slow_count; i++) {
			GPIO_InitStruct.GPIO_Pin = adc_map[adc_config.slow_channel[i]].pin;
			GPIO_Init(adc_map[adc_config.slow_channel[i]].gpio, &GPIO_InitStruct);
		}
		// DMA2 Stream0 -> ADC1
		DMA_InitStruct.DMA_Channel = DMA_Channel_0;
		DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) 0x4001204C;
		DMA_InitStruct.DMA_Memory0BaseAddr =
				(uint32_t) &(adc.raw[ADC_FAST_COUNT]);// start at end of fast count
		DMA_InitStruct.DMA_BufferSize = adc_config.slow_count;
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
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;	// SCAN: convert 0-n channels
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
	ADC_InitStruct.ADC_NbrOfConversion = adc_config.slow_count;
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	// ADC1 slow trigger: PWM or SW
	if ((adc_config.slow_pwm_trigger) && (adc_config.slow_count != 0)) {
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
	if (adc_config.slow_count > 0) {
		// ADC1 sequence config
		for (i = 0; i < adc_config.slow_count; i++) {
			ADC_RegularChannelConfig(ADC1, adc_config.slow_channel[i], i + 1,
			ADC_SAMPLE_TIME);
		}

		// ADC1 continue issuing dma requests
		ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

		// ADC1 enable dma
		ADC_DMACmd(ADC1, ENABLE);
	}
	// END OF SLOW SEQUENCE CONFIGURATION

	// FAST SEQUENCE CONFIGURATION
	if (adc_config.fast_count[0] != 0) {
		// injected config
		// Add an extra conversion so that interrupt is guaranteed to
		// be generated after ADC2 and ADC3 are done with conversion
		ADC_InjectedSequencerLengthConfig(ADC1, adc_config.fast_count[0]);
		ADC_InjectedChannelConfig(ADC1, adc_config.fast_channel[0][0], 1,
		ADC_SAMPLE_TIME);
		ADC_InjectedChannelConfig(ADC1, adc_config.fast_channel[0][1], 2,
		ADC_SAMPLE_TIME);
		ADC_InjectedChannelConfig(ADC1, adc_config.fast_channel[0][2], 3,
		ADC_SAMPLE_TIME);
		ADC_InjectedChannelConfig(ADC1, adc_config.fast_channel[0][3], 4,
		ADC_SAMPLE_TIME);
		ADC_InjectedSequencerLengthConfig(ADC2, adc_config.fast_count[1]);
		ADC_InjectedChannelConfig(ADC2, adc_config.fast_channel[1][0], 1,
		ADC_SAMPLE_TIME);
		ADC_InjectedChannelConfig(ADC2, adc_config.fast_channel[1][1], 2,
		ADC_SAMPLE_TIME);
		ADC_InjectedChannelConfig(ADC2, adc_config.fast_channel[1][2], 3,
		ADC_SAMPLE_TIME);
		ADC_InjectedChannelConfig(ADC2, adc_config.fast_channel[1][3], 4,
		ADC_SAMPLE_TIME);
		ADC_InjectedSequencerLengthConfig(ADC3, adc_config.fast_count[2]);
		ADC_InjectedChannelConfig(ADC3, adc_config.fast_channel[2][0], 1,
		ADC_SAMPLE_TIME);
		ADC_InjectedChannelConfig(ADC3, adc_config.fast_channel[2][1], 2,
		ADC_SAMPLE_TIME);
		ADC_InjectedChannelConfig(ADC3, adc_config.fast_channel[2][2], 3,
		ADC_SAMPLE_TIME);
		ADC_InjectedChannelConfig(ADC3, adc_config.fast_channel[2][3], 4,
		ADC_SAMPLE_TIME);

		// injected trigger
		if (adc_config.fast_pwm_trigger) {
			ADC_ExternalTrigInjectedConvEdgeConfig(ADC1,
			ADC_ExternalTrigInjecConvEdge_Rising);
			ADC_ExternalTrigInjectedConvEdgeConfig(ADC2,
			ADC_ExternalTrigInjecConvEdge_Rising);
			ADC_ExternalTrigInjectedConvEdgeConfig(ADC3,
			ADC_ExternalTrigInjecConvEdge_Rising);
		} else {
			ADC_ExternalTrigInjectedConvEdgeConfig(ADC1,
			ADC_ExternalTrigInjecConvEdge_None);
			ADC_ExternalTrigInjectedConvEdgeConfig(ADC2,
			ADC_ExternalTrigInjecConvEdge_None);
			ADC_ExternalTrigInjectedConvEdgeConfig(ADC3,
			ADC_ExternalTrigInjecConvEdge_None);
		}
		ADC_ExternalTrigInjectedConvConfig(ADC1,
		ADC_ExternalTrigInjecConv_T1_TRGO);
		ADC_ExternalTrigInjectedConvConfig(ADC2,
		ADC_ExternalTrigInjecConv_T1_TRGO);
		ADC_ExternalTrigInjectedConvConfig(ADC3,
		ADC_ExternalTrigInjecConv_T1_TRGO);

		// JADC interrupt
		ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
	}
	// END OF FAST SEQUENCE CONFIGURATION

	// enable interrupts
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);

	// start ADC
	if ((adc_config.fast_count[0] != 0) || (adc_config.slow_count != 0))
		ADC_Cmd(ADC1, ENABLE);
	if (adc_config.fast_count[1] != 0)
		ADC_Cmd(ADC2, ENABLE);
	if (adc_config.fast_count[2] != 0)
		ADC_Cmd(ADC3, ENABLE);

	// success
	return 0;
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
		return adc.raw[channel];
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
	int i, j;
	// clear flag
	ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
	// store converted values
	j = 0;
	// ADC1
	for (i = 0; i < adc_config.fast_count[0]; i++) {
		adc.raw[j] = ADC_GetInjectedConversionValue(ADC1,
		ADC_InjectedChannel_1 + 4 * i);
		j++;
	}
	// ADC2
	for (i = 0; i < adc_config.fast_count[1]; i++) {
		adc.raw[j] = ADC_GetInjectedConversionValue(ADC2,
		ADC_InjectedChannel_1 + 4 * i);
		j++;
	}
	// ADC3
	for (i = 0; i < adc_config.fast_count[2]; i++) {
		adc.raw[j] = ADC_GetInjectedConversionValue(ADC3,
		ADC_InjectedChannel_1 + 4 * i);
		j++;
	}
	// call handler
	if (adc.fast_handler != 0) {
		adc.fast_handler();
	}
}

