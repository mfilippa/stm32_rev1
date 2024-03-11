// -----------------------------------------------------------------------------
// io.h - Mariano F - 03/2024
// -----------------------------------------------------------------------------

#ifndef _IO_H_
#define _IO_H_

// public includes
#include <stdint.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// io channels
typedef enum io_channel_enum {
    IO_CH_LED,               // LED
    IO_CH_DEBUG1,            // DEBUG1
    IO_CH_DEBUG2,            // DEBUG2
    IO_CH_COUNT,
} io_ch_t;

// io states
typedef enum io_states_enum {
    IO_STATE_RESET = 0,
    IO_STATE_SET,
} io_state_t;

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init
void io_init(void);

// set channel
void io_set(io_ch_t channel);

// reset channel
void io_reset(io_ch_t channel);

// toggle channel
io_state_t io_toggle(io_ch_t channel);

// read channel
io_state_t io_read(io_ch_t channel);

// comm handler
void io_comm_handler(uint32_t rx_size);

#endif // _IO_H_
