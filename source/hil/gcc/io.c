// -----------------------------------------------------------------------------
// io.c - MPF - 10/2019
// -----------------------------------------------------------------------------

// includes
#include "error.h"
#include "io.h"
#include <stdio.h>

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
    if (channel<IO_CH_COUNT) io.ch_state[channel] = IO_STATE_SET;
    else error_raise(ERROR_IO_ARG);
    // output to console
    if(channel==IO_CH_LED) printf("O");
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(io_ch_t channel){
    if (channel<IO_CH_COUNT) io.ch_state[channel] = IO_STATE_RESET;
    else error_raise(ERROR_IO_ARG);
    // output to console
    if(channel==IO_CH_LED) printf("-");
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
io_state_t io_toggle(io_ch_t channel){
    io_state_t state = IO_STATE_RESET;
    if (channel<IO_CH_COUNT) {
        if (io.ch_state[channel]==IO_STATE_RESET) {
            io_set(channel);
            state = IO_STATE_SET;
        } 
        else {
            io_reset(channel);
            state = IO_STATE_RESET;
        }
    }
    else error_raise(ERROR_IO_ARG);
    return state;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
io_state_t io_read(io_ch_t channel){
    io_state_t state = IO_STATE_RESET;
    if (channel<IO_CH_COUNT) state = io.ch_state[channel];
    else error_raise(ERROR_IO_ARG);
    return state;
}