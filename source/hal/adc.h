// -----------------------------------------------------------------------------
// adc.h - MPF 11/2019
// -----------------------------------------------------------------------------

#ifndef _ADC_H_
#define _ADC_H_

// includes
#include <stdint.h>
#include <stdio.h>

// ADC channels - do not use physical ifaces, use hw abstracted names
typedef enum adc_channels_enum {
    ADC_CH1=0,
    ADC_CH2,
    ADC_CH3,
    ADC_CH4,
    ADC_CH5,
    ADC_CH6,
    ADC_CH7,
    ADC_CH8,
    ADC_CH9,
    ADC_TEMP,
    ADC_CH_COUNT,
} adc_channel_t;

// init: intializes the module
// Two groups expected: fast conversion and slow conversion
// Pass function pointers if interrupts are to be generated after
// a group conversion, or NULL to disable that interrupt
void adc_init(void (*slow_handler)(void), void (*fast_handler)(void));

// force an ADC trigger
void adc_sw_trigger_slow(void);
void adc_sw_trigger_fast(void);

// reads the ADC value
uint32_t adc_read(adc_channel_t channel);

#endif // _ADC_H_
