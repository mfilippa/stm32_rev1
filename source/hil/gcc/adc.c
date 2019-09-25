// -----------------------------------------------------------------------------
// adc.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "hal/adc.h"

// module structure
struct adc_struct {

} adc;

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t adc_init(adc_config_t * config, void (*slow_handler)(void), 
    void (*fast_handler)(void)) {
    // success
    return 0;
}

// -----------------------------------------------------------------------------
// trigger slow
// -----------------------------------------------------------------------------
void adc_sw_trigger_slow(void) {

}

// -----------------------------------------------------------------------------
// trigger fast
// -----------------------------------------------------------------------------
void adc_sw_trigger_fast(void) {

}

// -----------------------------------------------------------------------------
// ADC read
// -----------------------------------------------------------------------------
uint32_t adc_read(uint32_t channel) {
    return 0;
}

// -----------------------------------------------------------------------------
// ADC fast handler
// -----------------------------------------------------------------------------
void adc_fast_handler(void){
    // call handler

}

// -----------------------------------------------------------------------------
// ADC slow handler
// -----------------------------------------------------------------------------
void adc_slow_handler(void){
    // call handler

}



