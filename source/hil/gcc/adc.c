// -----------------------------------------------------------------------------
// adc.c - MPF 11/2021
// -----------------------------------------------------------------------------

// includes
#include "adc.h"

// module structure
struct adc_struct {
    // handlers
    void (*slow_handler)(void);
    void (*fast_handler)(void);
    // raw data storage
    uint32_t fast_raw[ADC_FAST_COUNT];
    uint32_t slow_raw[ADC_SLOW_COUNT];
} adc;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void adc_init(void (*slow_handler)(void), void (*fast_handler)(void)){ 
    adc.fast_handler = fast_handler;
    adc.slow_handler = slow_handler;
    for (int i=0; i<ADC_FAST_COUNT; i++) adc.fast_raw[i] = 0;
    for (int i=0; i<ADC_SLOW_COUNT; i++) adc.slow_raw[i] = 0;
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
    return adc.fast_raw[channel];
}
uint32_t adc_read_slow(uint32_t channel) {
    return adc.slow_raw[channel];
}