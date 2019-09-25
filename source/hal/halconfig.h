// -----------------------------------------------------------------------------
// halconfig.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _HALCONFIG_H_
#define _HALCONFIG_H_

// io channels
enum io_channel_enum {
    IO_LED = 0,           // LED
    IO_DEBUG,             // DEBUG
    IO_CH_COUNT,
};

// ADC channels
enum adc_channels_enum {
    // fast channels
    ADC_FAST_CH1=0,
    ADC_FAST_CH2,
    ADC_FAST_CH3,
    ADC_FAST_CH4,
    // slow channels
    ADC_SLOW_CH1,
    ADC_SLOW_CH2,
    ADC_SLOW_CH3,
    ADC_SLOW_CH4,
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
    // slow channels
    FB_SLOW_CH1,
    FB_SLOW_CH2,
    FB_SLOW_CH3,
    FB_SLOW_CH4,
    FB_TEMP,
    FB_CH_COUNT,
};


#endif // _HALCONFIG_H_
