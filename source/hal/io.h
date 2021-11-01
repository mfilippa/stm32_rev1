// -----------------------------------------------------------------------------
// io.h - MPF 10/2021
// -----------------------------------------------------------------------------

#ifndef _IO_H_
#define _IO_H_

// includes
#include <stdint.h>
#include <stdio.h>

// io channels - do not use physical ifaces, use hw abstracted names
typedef enum io_channel_enum {
    IO_CH_LED = 0,           // LED
    IO_CH_DEBUG1,            // DEBUG1
    IO_CH_DEBUG2,            // DEBUG2
    IO_CH_COUNT,
} io_ch_t;

// io states
typedef enum io_states_enum {
    IO_STATE_RESET = 0,
    IO_STATE_SET,
} io_state_t;

// *** for simulation environment only ***
// io data
typedef struct io_data_struct {
    io_state_t state[IO_CH_COUNT];
} io_data_t;
extern io_data_t io_data;
// *** end simulation environment data ***

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
