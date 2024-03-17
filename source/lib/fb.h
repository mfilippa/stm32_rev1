// -----------------------------------------------------------------------------
// fb.h - Mariano F - 03/2024
// -----------------------------------------------------------------------------

#ifndef _FB_H_
#define _FB_H_

// public includes
#include <stdint.h>
#include <float.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// fb table
typedef struct fb_table_struct {
    float offset;             // raw ADC offset
    float scaling;            // scaling
    float value;              // converted value
    float min;                // min value to check against
    float max;                // max value to check against
} fb_table_t;

// feedback channels
typedef enum fb_channel_enum {
    FB_CH1,
    FB_CH2,
    FB_CH3,
    FB_CH4,
    FB_CH_COUNT,
} fb_ch_t;

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init
void fb_init(void);

// process: calculate final value based on offset and scaling
void fb_process(fb_ch_t channel, uint32_t raw);

// get: get final value
float fb_get(fb_ch_t channel);

// range check: check if value is out of range (max/min)
bool fb_out_of_range_min_max(fb_ch_t channel);
bool fb_out_of_range_min(fb_ch_t channel);
bool fb_out_of_range_max(fb_ch_t channel);

// store_offset
void fb_store_offset(fb_ch_t channel, float offset);

// comm handler
void fb_comm_handler(uint32_t rx_size);

#endif // _FB_H_
