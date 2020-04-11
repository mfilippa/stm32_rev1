// -----------------------------------------------------------------------------
// adc.c - MPF 11/2019
// -----------------------------------------------------------------------------

// includes
#include "error.h"
#include "adc.h"

// adc buffers
uint32_t adc_buffer[ADC_CH_COUNT];

// module structure
struct adc_struct {
    void (*fast_handler)(void);
    void (*slow_handler)(void);
} adc;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void adc_init(void (*slow_handler)(void), void (*fast_handler)(void)){ 
    adc.fast_handler = fast_handler;
    adc.slow_handler = slow_handler;
    for (int i=0; i<ADC_CH_COUNT; i++) adc_buffer[i] = 0;
}

// -----------------------------------------------------------------------------
// sw trigger
// -----------------------------------------------------------------------------
void adc_sw_trigger(void) {
    // nothing to do
}

// -----------------------------------------------------------------------------
// ADC read
// -----------------------------------------------------------------------------
uint32_t adc_read(uint32_t channel) {
    uint32_t value = 0;
    if (channel<ADC_CH_COUNT) value = adc_buffer[channel];
    else error_raise(ERROR_ADC_ARG);
    return value;
}