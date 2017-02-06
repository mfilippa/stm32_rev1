// -----------------------------------------------------------------------------
// app.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _APP_H_
#define _APP_H_

// system includes
#include <stdint.h>

// ADC channels
enum adc_channels {
	ADC_FAST_CH1=0,
	ADC_FAST_CH2,
	ADC_FAST_CH3,
	ADC_FAST_CH4,
	ADC_FAST_CH5,
	ADC_FAST_CH6,
	ADC_FAST_CH7,
	ADC_FAST_CH8,
	ADC_FAST_CH9,
	ADC_FAST_CH10,
	ADC_SLOW_CH1,
	ADC_SLOW_CH2,
	ADC_SLOW_CH3,
	ADC_SLOW_CH4,
	ADC_SLOW_CH5,
	ADC_SLOW_CH6,
	ADC_SLOW_CH7,
	ADC_SLOW_CH8,
	ADC_CH_COUNT,
};

// init: initializes application, returns 0 if successful
uint32_t app_init(void);

// background: background step
void app_background(void);

// systick: to be executed at system timer tick event
void app_systick(void);

// slow adc processing
void app_adc_process_slow(void);

// fast adc processing
void app_adc_process_fast(void);

#endif // _APP_H_
