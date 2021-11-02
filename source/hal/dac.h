// -----------------------------------------------------------------------------
// dac.h - MPF 10/2021
// -----------------------------------------------------------------------------

#ifndef _DAC_H_
#define _DAC_H_

// includes
#include <stdint.h>
#include <stdio.h>

// dac channels
typedef enum dac_channel_enum {
    DAC_CH1 = 0,
    DAC_CH2,
    DAC_CH_COUNT,
} dac_ch_t;

// trigger
typedef enum dac_trigger_enum {
    DAC_TRIGGER_NONE = 0,   // DAC values loaded immediately
    DAC_TRIGGER_PWM,        // DAC values loaded by PWM trigger
    DAC_TRIGGER_SW,         // DAC values loaded by SW trigger
    DAC_TRIGGER_COUNT
} dac_trigger_t;

// *** for simulation environment only ***
// dac data
typedef struct dac_data_struct {
    int32_t dac_ch1_value;  // 0..2^16-1
    int32_t dac_ch2_value;  // 0..2^16-1
} dac_data_t;
extern dac_data_t dac_data;
// *** end simulation environment data ***

// init: initializes module
void dac_init(dac_trigger_t trigger);

// set: sets channel, value = 0..2^16-1
void dac_set(dac_ch_t channel, int32_t value);

// software trigger
void dac_sw_trigger(void);

#endif // _DAC_H_
