// -----------------------------------------------------------------------------
// adc.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _ADC_H_
#define _ADC_H_

// system includes
#include <stdint.h>

// fast adc channel enumerations
typedef enum  adc_fast_enum {
	ADC_FAST_CH1 = 0,
	ADC_FAST_CH2,
	ADC_FAST_CH3,
	ADC_FAST_CH4,
	ADC_FAST_CH5,
	ADC_FAST_CH6,
	ADC_FAST_CH7,
	ADC_FAST_CH8,
	ADC_FAST_COUNT,
} adc_fast_t;

// slow adc channel enumerations
typedef enum  adc_slowch_enum {
	ADC_SLOW_CH1 = 0,
	ADC_SLOW_CH2,
	ADC_SLOW_CH3,
	ADC_SLOW_CH4,
//	ADC_SLOW_CH5,
//	ADC_SLOW_CH6,
//	ADC_SLOW_CH7,
//	ADC_SLOW_CH8,
	ADC_SLOW_COUNT,
} adc_slow_t;

// init: intializes the module, return 0 if successful
// pass function pointers if interrupts are to be generated after
// a group conversion, or ZERO to disable that interrupt
uint32_t adc_init(void (*slow_handler)(void), void (*fast_handler)(void));

// read_fast: reads the ADC value from the fast group
uint32_t adc_read_fast(adc_fast_t channel);

// read_slow: reads the ADC value from the slow group
uint32_t adc_read_slow(adc_slow_t channel);

// trigger_slow: force an ADC trigger on slow group
void adc_trigger_slow(void);

// trigger_fast: force an ADC trigger on fast group
void adc_trigger_fast(void);

#endif // _ADC_H_
