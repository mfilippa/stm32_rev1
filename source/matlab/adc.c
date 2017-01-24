// -----------------------------------------------------------------------------
// adc.c - MPF - 10/2013
// -----------------------------------------------------------------------------

// includes
#include "wrapper.h"
#include "hal/halinc.h"


// module structure
struct adc_struct {
	void (*slow_handler)(void);
	void (*fast_handler)(void);
	uint32_t raw_fast[ADC_FAST_COUNT];
	uint32_t raw_slow[ADC_SLOW_COUNT];
} adc;

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t adc_init(void (*slow_handler)(void), void (*fast_handler)(void)){

	// store handlers
	adc.slow_handler = slow_handler;
	adc.fast_handler = fast_handler;

	return 0;
}

// -----------------------------------------------------------------------------
// read slow
// -----------------------------------------------------------------------------
uint32_t adc_read_slow(adc_slow_t channel){
	if (channel>=ADC_SLOW_COUNT) return 0;
	else return adc.raw_slow[channel];
}

// -----------------------------------------------------------------------------
// read fast
// -----------------------------------------------------------------------------
uint32_t adc_read_fast(adc_fast_t channel){
	if (channel>=ADC_FAST_COUNT) return 0;
	else return adc.raw_fast[channel];
}

// -----------------------------------------------------------------------------
// matlab slow - this function reads slow channels from matlab
// -----------------------------------------------------------------------------
void adc_matlab_slow(void){
	// read slow raw channels here
	adc.raw_slow[ADC_SLOW_CH1]   = pin[10];
	adc.raw_slow[ADC_SLOW_CH2]   = pin[11];
	adc.raw_slow[ADC_SLOW_CH3]   = pin[12];
	adc.raw_slow[ADC_SLOW_CH4]   = pin[13];
	// call handler
	if (adc.slow_handler != 0){
		adc.slow_handler();
	}
}

// -----------------------------------------------------------------------------
// matlab fast - this function reads fast channels from matlab
// -----------------------------------------------------------------------------
void adc_matlab_fast(void){
	// read fast raw channels here
	adc.raw_fast[ADC_FAST_CH1] = pin[0];
	adc.raw_fast[ADC_FAST_CH2] = pin[1];
	adc.raw_fast[ADC_FAST_CH3] = pin[2];
	adc.raw_fast[ADC_FAST_CH4] = pin[3];
	adc.raw_fast[ADC_FAST_CH5] = pin[4];
	adc.raw_fast[ADC_FAST_CH6] = pin[5];
	adc.raw_fast[ADC_FAST_CH7] = pin[6];
	adc.raw_fast[ADC_FAST_CH8] = pin[7];
	// call handler
	if (adc.fast_handler != 0){
		adc.fast_handler();
	}
}

// -----------------------------------------------------------------------------
// trigger_slow
// -----------------------------------------------------------------------------
void adc_trigger_slow(void){
	adc_matlab_slow();
}

// -----------------------------------------------------------------------------
// trigger_fast
// -----------------------------------------------------------------------------
void adc_trigger_fast(void){
	adc_matlab_fast();
}
