// -----------------------------------------------------------------------------
// halconfig.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _HALCONFIG_H_
#define _HALCONFIG_H_

// io channels
enum io_channel_enum {
    IO_LED = 0,            // LED
    IO_DEBUG1,            // DEBUG1
    IO_DEBUG2,            // DEBUG2
    IO_CH_COUNT,
};

// ADC channels
enum adc_channels_enum {
    // fast channels
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
    // slow channels
    ADC_SLOW_CH1,
    ADC_SLOW_CH2,
    ADC_SLOW_CH3,
    ADC_SLOW_CH4,
    ADC_SLOW_CH5,
    ADC_SLOW_CH6,
    ADC_SLOW_CH7,
    ADC_SLOW_TEMP,
    ADC_CH_COUNT,
};

// feedback channels
enum fb_channel_enum {
    // fast channels
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
    // slow channels
    FB_SLOW_CH1,
    FB_SLOW_CH2,
    FB_SLOW_CH3,
    FB_SLOW_CH4,
    FB_SLOW_CH5,
    FB_SLOW_CH6,
    FB_SLOW_CH7,
    FB_TEMP,
    FB_CH_COUNT,
};


#endif // _HALCONFIG_H_
