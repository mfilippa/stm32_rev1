// -----------------------------------------------------------------------------
// fb.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _FB_H_
#define _FB_H_

// includes
#include <stdint.h>

// fb channel enumerations
typedef enum  fb_channel_enum {
	// fast adc
	FB_FAST_CH1 = 0,
	FB_FAST_CH2,
	FB_FAST_CH3,
	FB_FAST_CH4,
	FB_FAST_CH5,
	FB_FAST_CH6,
	FB_FAST_CH7,
	FB_FAST_CH8,
	// slow adc
	FB_SLOW_CH1,
	FB_SLOW_CH2,
	FB_SLOW_CH3,
	FB_SLOW_CH4,
	FB_CHANNEL_COUNT
} fb_channel_t;

// init: initializes module, returns 0
uint32_t fb_init(void);

// store_params: store processing parameters
void fb_store_params(fb_channel_t channel, int32_t scaling, int32_t scaling_q,
		int32_t min, int32_t max);

// get_raw: gets raw ADC value
uint32_t fb_get_raw(fb_channel_t channel);

// store_offset: store offset, if any. Use fb_get_raw() to calculate
// an offset to store
void fb_store_offset(fb_channel_t channel, uint32_t raw);

// process: calculate final value based on offset and scaling
void fb_process(fb_channel_t channel, int32_t raw);

// get: get final value
int32_t fb_get(fb_channel_t channel);

// check: check if value is out of range (max/min), return 1 if so, 0 if not.
uint32_t fb_range_check(fb_channel_t channel);

#endif // _FB_H_
