// -----------------------------------------------------------------------------
// fb.c - Mariano F - 03/2024
// -----------------------------------------------------------------------------

// includes
#include "fb.h"

// simulation variables
#ifdef SIMULATION
extern double * pin;
extern double * pout;
#endif

// private defines
#define FB_DEF_OFFSET   (0ul)
#define FB_DEF_SCALING  (1.0f)
#define FB_DEF_MIN      (FLT_MIN)
#define FB_DEF_MAX      (FLT_MAX)

// module struct
struct fb_struct {
    fb_table_t table[FB_CH_COUNT];
} fb;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void fb_init(void){

    for (int i=0; i< FB_CH_COUNT; i++){
        fb.table[i].offset = FB_DEF_OFFSET;
        fb.table[i].scaling = FB_DEF_SCALING;
        fb.table[i].min = FB_DEF_MIN;
        fb.table[i].max = FB_DEF_MAX;
    }
}

// -----------------------------------------------------------------------------
// process
// -----------------------------------------------------------------------------
void fb_process(fb_ch_t channel, uint32_t raw){
    if(channel<FB_CH_COUNT) {
        fb.table[channel].value = ((float)raw-fb.table[channel].offset)*
            fb.table[channel].scaling;
    }
}

// -----------------------------------------------------------------------------
// get
// -----------------------------------------------------------------------------
float fb_get(fb_ch_t channel){
    return (channel<FB_CH_COUNT)?fb.table[channel].value:0;
}

//------------------------------------------------------------------------------
// check: check if value is out of range (max/min)
//------------------------------------------------------------------------------
bool fb_out_of_range_min_max(fb_ch_t channel){
    return (channel<FB_CH_COUNT)?
        (fb.table[channel].value>fb.table[channel].max)||
        (fb.table[channel].value<fb.table[channel].min):
        true;
}
bool fb_out_of_range_min(fb_ch_t channel){
    return (channel<FB_CH_COUNT)?
        (fb.table[channel].value<fb.table[channel].min):
        true;
}
bool fb_out_of_range_max(fb_ch_t channel){
    return (channel<FB_CH_COUNT)?
        (fb.table[channel].value>fb.table[channel].max):
        true;
}

// -----------------------------------------------------------------------------
// store offset
// -----------------------------------------------------------------------------
void fb_store_offset(fb_ch_t channel, float offset){
    if (channel < FB_CH_COUNT) {
        fb.table[channel].offset = offset;
    }
}
