// -----------------------------------------------------------------------------
// adc.c - MPF 11/2021
// -----------------------------------------------------------------------------

// includes
#include "adc.h"

// adc data
adc_data_t adc_data;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void adc_init(void (*fast_handler)(void), void (*slow_handler)(void)){ 
    adc_data.fast_handler = fast_handler;
    adc_data.slow_handler = slow_handler;
    for (int i=0; i<ADC_FAST_COUNT; i++) adc_data.fast_raw[i] = 0;
    for (int i=0; i<ADC_SLOW_COUNT; i++) adc_data.slow_raw[i] = 0;
}

// -----------------------------------------------------------------------------
// sw trigger
// -----------------------------------------------------------------------------
void adc_sw_trigger_fast(void) {
    // nothing to do
}
void adc_sw_trigger_slow(void) {
    // nothing to do
}

// -----------------------------------------------------------------------------
// ADC read
// -----------------------------------------------------------------------------
uint32_t adc_read_fast(uint32_t channel) {
    return adc_data.fast_raw[channel];
}
uint32_t adc_read_slow(uint32_t channel) {
    return adc_data.slow_raw[channel];
}