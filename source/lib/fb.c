// -----------------------------------------------------------------------------
// fb.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "fb.h"

// module struct
struct fb_struct {
	fb_table_t * table;
	uint32_t tsize;
} fb;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t fb_init(fb_table_t * fb_table, uint32_t tsize){
	uint32_t i;
	if (tsize==0) return 1;

	// store data
	fb.table = fb_table;
	fb.tsize = tsize;

	// zero everything
	for (i=0; i< fb.tsize; i++){
		fb.table[i].offset = fb.table[i].raw = fb.table[i].value = 0;
		fb.table[i].min = fb.table[i].max = fb.table[i].out_of_range = 0;
	}
	return 0;
}
// -----------------------------------------------------------------------------
// process
// -----------------------------------------------------------------------------
void fb_process(uint32_t channel, int32_t raw){
	int32_t temp;
	if (channel >= fb.tsize) return;
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
uint32_t fb_range_check(uint32_t channel){
	if (channel >= fb.tsize) return 0;
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
void fb_store_offset(uint32_t channel, uint32_t raw){
	if (channel >= fb.tsize) return;
	fb.table[channel].offset = raw;
}
// -----------------------------------------------------------------------------
// store offset
// -----------------------------------------------------------------------------
void fb_store_params(uint32_t channel, int32_t scaling, int32_t scaling_q,
		int32_t min, int32_t max){
	if (channel >= fb.tsize) return;
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
int32_t fb_get(uint32_t channel){
	if (channel >= fb.tsize) return 0;
	return fb.table[channel].value;
}

// -----------------------------------------------------------------------------
// get raw
// -----------------------------------------------------------------------------
uint32_t fb_get_raw(uint32_t channel){
	if (channel >= fb.tsize) return 0;
	return fb.table[channel].raw;
}
