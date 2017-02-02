// -----------------------------------------------------------------------------
// adc.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _ADC_H_
#define _ADC_H_

// system includes
#include <stdint.h>

// ADC channels
typedef enum adc_channels_enum {
	// fast channels
	ADC_CH_FAST1,
	ADC_CH_FAST2,
	ADC_CH_FAST3,
	ADC_CH_FAST4,
	ADC_CH_FAST5,
	ADC_CH_FAST6,
	ADC_CH_FAST7,
	ADC_CH_FAST8,
	ADC_CH_FAST9,
	ADC_CH_FAST10,
	// slow channels
	ADC_CH_SLOW1,
	ADC_CH_SLOW2,
	ADC_CH_SLOW3,
	ADC_CH_SLOW4,
	ADC_CH_SLOW5,
	ADC_CH_SLOW6,
	ADC_CH_SLOW7,
	ADC_CH_SLOW8,
	ADC_CH_COUNT,
} adc_channel_t;


// init: intializes the module, return 0 if successful
// pass function pointers if interrupts are to be generated after
// a group conversion, or ZERO to disable that interrupt
uint32_t adc_init(void (*slow_handler)(void), void (*fast_handler)(void));

// force an ADC trigger, fast group
void adc_sw_trigger_fast(void);

// force an ADC trigger, slow group
void adc_sw_trigger_slow(void);

// reads the ADC value
uint32_t adc_read(adc_channel_t channel);

#endif // _ADC_H_
