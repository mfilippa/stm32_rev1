// -----------------------------------------------------------------------------
// adc.c - MPF - 10/2013
// -----------------------------------------------------------------------------

// includes
#include "wrapper.h"
#include "hal/halinc.h"


// module structure
struct adc_struct {
	// adc configuration
	adc_config_t * config;
	// handlers
	void (*slow_handler)(void);
	void (*fast_handler)(void);
} adc;

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t adc_init(adc_config_t * config, void (*slow_handler)(void), void (*fast_handler)(void)){

	// store data
	adc.slow_handler = slow_handler;
	adc.fast_handler = fast_handler;
	adc.config = config;

	return 0;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
uint32_t adc_read(uint32_t channel){
	if (channel>=adc.config->nr_channels) return 0;
	else return pin[adc.config->pin_start+channel];
}

// -----------------------------------------------------------------------------
// trigger_slow
// -----------------------------------------------------------------------------
void adc_sw_trigger_slow(void){
	adc.slow_handler();
}

// -----------------------------------------------------------------------------
// trigger_fast
// -----------------------------------------------------------------------------
void adc_sw_trigger_fast(void){
	adc.fast_handler();
}
