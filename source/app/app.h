// -----------------------------------------------------------------------------
// app.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _APP_H_
#define _APP_H_

// system includes
#include <stdint.h>

// feedback channels
enum fb_channel_enum {
	FB_FAST_CH1=0,
	FB_FAST_CH2,
	FB_FAST_CH3,
	FB_FAST_CH4,
	FB_FAST_CH5,
	FB_FAST_CH6,
	FB_FAST_CH7,
	FB_FAST_CH8,
	FB_FAST_CH9,
	FB_FAST_CH10,
	FB_SLOW_CH1,
	FB_SLOW_CH2,
	FB_SLOW_CH3,
	FB_SLOW_CH4,
	FB_SLOW_CH5,
	FB_SLOW_CH6,
	FB_SLOW_CH7,
	FB_SLOW_CH8,
	FB_CH_COUNT,
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
