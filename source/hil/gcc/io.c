// -----------------------------------------------------------------------------
// io.c - MPF - 11/2021
// -----------------------------------------------------------------------------

// includes
#include "io.h"

// module struct
struct io_struct {
    io_state_t ch_state[IO_CH_COUNT];
} io;

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
void io_init(void){

}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void io_set(io_ch_t channel){
    io.ch_state[channel]=IO_STATE_SET;
    // output to console
    if(channel==IO_CH_LED) printf("O");
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(io_ch_t channel){
    io.ch_state[channel]=IO_STATE_RESET;
    // output to console
    if(channel==IO_CH_LED) printf("-");
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
io_state_t io_toggle(io_ch_t channel){
    io_state_t state = IO_STATE_RESET;
    if (io.ch_state[channel]==IO_STATE_RESET) {
        io_set(channel);
        state = IO_STATE_SET;
    } 
    else {
        io_reset(channel);
        state = IO_STATE_RESET;
    }
    return state;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
io_state_t io_read(io_ch_t channel){
    return io.ch_state[channel];
}