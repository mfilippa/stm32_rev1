// -----------------------------------------------------------------------------
// io.h - MPF 11/2019
// -----------------------------------------------------------------------------

#ifndef _IO_H_
#define _IO_H_

// includes
#include <stdint.h>
#include <stdio.h>

// io channels - do not use physical ifaces, use hw abstracted names
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

// init: initializes module
void io_init(void);

// set: sets channel
void io_set(io_ch_t channel);

// reset: resets channel
void io_reset(io_ch_t channel);

// toggle: toggles channel
io_state_t io_toggle(io_ch_t channel);

// read: reads channel
io_state_t io_read(io_ch_t channel);

#endif // _IO_H_
