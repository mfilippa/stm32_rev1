// -----------------------------------------------------------------------------
// dac.h - Mariano F - 04/2023
// -----------------------------------------------------------------------------

#ifndef _DAC_H_
#define _DAC_H_

// public includes
#include <stdint.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// dac channels
typedef enum dac_ch_enum {
    DAC_CH1 = 0,
    DAC_CH2,
    DAC_CH_COUNT,
} dac_ch_t;

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init
void dac_init(void);

// set a channel
void dac_set(dac_ch_t channel, float value);

// trigger a dac out
void dac_trigger(void);

// comm handler
void dac_comm_handler(uint32_t rx_size);

#endif // _DAC_H_
