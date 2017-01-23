// -----------------------------------------------------------------------------
// fb.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "fb.h"

// fb table
typedef struct fb_table_struct {
	int32_t raw;				// raw ADC value
	int32_t offset;				// raw ADC offset
	int32_t scaling;			// scaling
	int32_t scaling_q;			// q factor for scaling
	int32_t value;				// converted value
	int32_t min;				// min value to check against
	int32_t max;				// max value to check against
	int32_t out_of_range;		// out of range flag if value exceeds max/min
} fb_table_t;

// module struct
struct fb_struct {
	fb_table_t table[FB_CHANNEL_COUNT];
} fb;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t fb_init(void){
	uint32_t i;
	// zero everything
	for (i=0; i< FB_CHANNEL_COUNT-1; i++){
		fb.table[i].offset = fb.table[i].raw = fb.table[i].value = 0;
		fb.table[i].min = fb.table[i].max = fb.table[i].out_of_range = 0;
	}
	return 0;
}
// -----------------------------------------------------------------------------
// process
// -----------------------------------------------------------------------------
void fb_process(fb_channel_t channel, int32_t raw){
	int32_t temp;
	if (channel >= FB_CHANNEL_COUNT) return;
	fb.table[channel].raw = raw;
	temp = fb.table[channel].raw - fb.table[channel].offset;
	temp *= fb.table[channel].scaling;
	if (fb.table[channel].scaling_q>0) fb.table[channel].value = temp >> fb.table[channel].scaling_q;
	else if (fb.table[channel].scaling_q<0) fb.table[channel].value = temp << (-fb.table[channel].scaling_q);
	else fb.table[channel].value = temp;
}
// -----------------------------------------------------------------------------
// check
// -----------------------------------------------------------------------------
uint32_t fb_range_check(fb_channel_t channel){
	if (channel >= FB_CHANNEL_COUNT) return 0;
	if ((fb.table[channel].value > fb.table[channel].max) ||
			(fb.table[channel].value < fb.table[channel].min)){
		fb.table[channel].out_of_range = 1;
	}
	else {
		fb.table[channel].out_of_range = 0;
	}
	return fb.table[channel].out_of_range;
}
// -----------------------------------------------------------------------------
// store offset
// -----------------------------------------------------------------------------
void fb_store_offset(fb_channel_t channel, uint32_t raw){
	if (channel >= FB_CHANNEL_COUNT) return;
	fb.table[channel].offset = raw;
}
// -----------------------------------------------------------------------------
// store offset
// -----------------------------------------------------------------------------
void fb_store_params(fb_channel_t channel, int32_t scaling, int32_t scaling_q,
		int32_t min, int32_t max){
	if (channel >= FB_CHANNEL_COUNT) return;
	fb.table[channel].scaling = scaling;
	fb.table[channel].scaling_q = scaling_q;
	if (min < max) {
		fb.table[channel].min = min;
		fb.table[channel].max = max;
	}
}
// -----------------------------------------------------------------------------
// get
// -----------------------------------------------------------------------------
int32_t fb_get(fb_channel_t channel){
	if (channel >= FB_CHANNEL_COUNT) return 0;
	return fb.table[channel].value;
}

// -----------------------------------------------------------------------------
// get raw
// -----------------------------------------------------------------------------
uint32_t fb_get_raw(fb_channel_t channel){
	if (channel >= FB_CHANNEL_COUNT) return 0;
	return fb.table[channel].raw;
}
