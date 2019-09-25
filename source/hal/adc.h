// -----------------------------------------------------------------------------
// adc.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _ADC_H_
#define _ADC_H_

// system includes
#include <stdint.h>

// STM32 internal temperature sensor (channel 16)
// Temp[degC] = ((VSENSE - V25) / AVG_SLOPE) + 25
// V25 = 0.76V
// AVG_SLOPE = 0.0025V/C

// defines
#define ADC_NR_CONVERTERS   3   // ADC1-3
#define ADC_MAX_FAST_COUNT  4   // for each ADC(1-3)
#define ADC_MAX_SLOW_COUNT  8   // total ADC1

// ADC configuration table
typedef struct adc_config_struct {
    // fast channel sequence: 0-15 for [0][n], 0-3 and 10-13 for [1-2][n]
    uint32_t fast_channel[ADC_NR_CONVERTERS][ADC_MAX_FAST_COUNT];
    // fast count: zero to disable, count[0] must be greater than count[1..2]
    uint32_t fast_count[ADC_NR_CONVERTERS];
    // trigger: 1 for pwm triggered, 0 for sw trigger
    uint32_t fast_pwm_trigger;
    // slow channel sequence: 0-15, 16 for temp, 17 for vrefint
    uint32_t slow_channel[ADC_MAX_SLOW_COUNT];
    // slow channel count, zero to disable
    uint32_t slow_count;
    // trigger: 1 for pwm, 0 for sw
    uint32_t slow_pwm_trigger;
} adc_config_t;

// init: intializes the module, return 0 if successful
// pass function pointers if interrupts are to be generated after
// a group conversion, or NULL to disable that interrupt
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
