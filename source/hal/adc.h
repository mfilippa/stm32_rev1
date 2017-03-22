// -----------------------------------------------------------------------------
// adc.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _ADC_H_
#define _ADC_H_

// system includes
#include <stdint.h>

// temperature sensor
// Temperature (in °C) = {(VSENSE – V25) / Avg_Slope} + 25
// V25 = 0.76V
// Avg_Slope = 2.5mV/C

#ifndef MATLAB
// ADC configuration table
typedef struct adc_config_struct {
	// fast channel sequence: 0-15 for [0][n], 0-3 and 10-13 for [1-2][n]
	uint32_t fast_channel[3][4];
	// fast count: zero to disable, [0] count > [1-2] count
	uint32_t fast_count[3];
	// trigger: 1 for pwm, 0 for sw
	uint32_t fast_pwm_trigger;
	// slow channel sequence: 0-15, 16 for temp, 17 for vrefint
	uint32_t slow_channel[8];
	// slow channel count, zero to disable
	uint32_t slow_count;
	// trigger: 1 for pwm, 0 for sw
	uint32_t slow_pwm_trigger;
} adc_config_t;
#else
// ADC configuration table
typedef struct adc_config_struct {
	// port (pin) start index
	uint32_t pin_start;
	// nr of channels
	uint32_t nr_channels;
} adc_config_t;
#endif

// init: intializes the module, return 0 if successful
// pass function pointers if interrupts are to be generated after
// a group conversion, or ZERO to disable that interrupt
uint32_t adc_init(adc_config_t * config, void (*slow_handler)(void), void (*fast_handler)(void));

// force an ADC trigger, fast group
void adc_sw_trigger_fast(void);

// force an ADC trigger, slow group
void adc_sw_trigger_slow(void);

// adc fast handler
void adc_fast_handler(void);

// adc slow handler
void adc_slow_handler(void);

// reads the ADC value
// order: fast_channel[x][x], slow_channel[x]
uint32_t adc_read(uint32_t channel);

#endif // _ADC_H_
