// -----------------------------------------------------------------------------
// adc.h - MPF 10/2021
// -----------------------------------------------------------------------------

#ifndef _ADC_H_
#define _ADC_H_

// includes
#include <stdint.h>
#include <stdio.h>

// ADC fast channels
typedef enum adc_fast_channels_enum {
    ADC_FAST_CH0=0,
    ADC_FAST_CH1,
    ADC_FAST_CH2,
    ADC_FAST_CH3,
    ADC_FAST_CH4,
    ADC_FAST_CH5,
    ADC_FAST_CH6,
    ADC_FAST_CH10,
    ADC_FAST_CH11,
    ADC_FAST_CH12,
    ADC_FAST_COUNT,
} adc_fast_channel_t;

// ADC slow channels
typedef enum adc_slow_channels_enum {
    ADC_SLOW_CH0=0,
    ADC_SLOW_CH1,
    ADC_SLOW_CH2,
    ADC_SLOW_CH3,
    ADC_SLOW_CH4,
    ADC_SLOW_CH5,
    ADC_SLOW_CH6,
    ADC_SLOW_TEMP,
    ADC_SLOW_COUNT,
} adc_slow_channel_t;

// *** for simulation environment only ***
// adc data
typedef struct adc_data_struct {
    uint32_t fast_raw[ADC_FAST_COUNT];
    uint32_t slow_raw[ADC_SLOW_COUNT];
    void (*slow_handler)(void);
    void (*fast_handler)(void);
} adc_data_t;
extern adc_data_t adc_data;
// *** end simulation environment data ***

// init: intializes the module
// Two groups expected: fast conversion and slow conversion
// Pass function pointers if interrupts are to be generated after
// a group conversion, or NULL to disable that interrupt
void adc_init(void (*fast_handler)(void), void (*slow_handler)(void));

// force a fast ADC trigger
void adc_sw_trigger_fast(void);

// force a slow ADC trigger
void adc_sw_trigger_slow(void);

// reads a fast ADC value
uint32_t adc_read_fast(adc_fast_channel_t channel);

// reads a slow ADC value
uint32_t adc_read_slow(adc_slow_channel_t channel);

#endif // _ADC_H_
