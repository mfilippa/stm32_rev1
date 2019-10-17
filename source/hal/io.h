// -----------------------------------------------------------------------------
// io.h - MPF 10/2019
// -----------------------------------------------------------------------------

#ifndef _IO_H_
#define _IO_H_

// system includes
#include <stdint.h>

// io channels
typedef enum io_channel_enum {
    IO_CH_LED = 0,           // LED
    IO_CH_DEBUG,             // DEBUG
    IO_CH_COUNT,
} io_ch_t;

// io states
typedef enum io_states_enum {
    IO_STATE_RESET = 0,
    IO_STATE_SET,
} io_state_t;

// init: initializes module, returns 0 if successful
uint32_t io_init(void);

// set: sets channel to ACTIVE state
void io_set(io_ch_t channel);

// reset: resets channel to INACTIVE state
void io_reset(io_ch_t channel);

// toggle: toggles channel, returns current state, ACTIVE as 1, INACTIVE as 0
io_state_t io_toggle(io_ch_t channel);

// read: reads channel, returns current state, ACTIVE as 1, INACTIVE as 0
io_state_t io_read(io_ch_t channel);

#endif // _IO_H_
