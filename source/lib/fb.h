// -----------------------------------------------------------------------------
// fb.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _FB_H_
#define _FB_H_

// includes
#include <stdint.h>

// fb table
typedef struct fb_table_struct {
    int32_t raw;                // raw ADC value
    int32_t offset;                // raw ADC offset
    int32_t scaling;            // scaling
    int32_t scaling_q;            // q factor for scaling
    int32_t value;                // converted value
    int32_t min;                // min value to check against
    int32_t max;                // max value to check against
    int32_t out_of_range;        // out of range flag if value exceeds max/min
} fb_table_t;

// init: initializes module, returns 0
uint32_t fb_init(fb_table_t * fb_table, uint32_t tsize);

// store_params: store processing parameters
void fb_store_params(uint32_t channel, int32_t scaling, int32_t scaling_q,
        int32_t min, int32_t max);

// get_raw: gets raw ADC value
uint32_t fb_get_raw(uint32_t channel);

// store_offset: store offset, if any. Use fb_get_raw() to calculate
// an offset to store
void fb_store_offset(uint32_t channel, uint32_t raw);

// process: calculate final value based on offset and scaling
void fb_process(uint32_t channel, int32_t raw);

// get: get final value
int32_t fb_get(uint32_t channel);

// check: check if value is out of range (max/min), return 1 if so, 0 if not.
uint32_t fb_range_check(uint32_t channel);

#endif // _FB_H_
