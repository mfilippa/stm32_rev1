// -----------------------------------------------------------------------------
// io.c - MPF - 11/2021
// -----------------------------------------------------------------------------

// includes
#include <stdio.h>
#include "io.h"

// io data
io_state_t io_channel[IO_CH_COUNT];

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
void io_init(void){

}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void io_set(io_ch_t channel){
    if (channel<IO_CH_COUNT) {
        io_channel[channel] = IO_STATE_SET;
        if(channel==IO_CH_LED) {
            printf("O");
        }
    }
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(io_ch_t channel){
    if (channel<IO_CH_COUNT) {
        io_channel[channel] = IO_STATE_RESET;
        if(channel==IO_CH_LED) {
            printf("-");
        }
    }
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
io_state_t io_toggle(io_ch_t channel){
    io_state_t state = IO_STATE_RESET;
    if (channel < IO_CH_COUNT) {
        if (io_channel[channel] == IO_STATE_RESET) {
            io_set(channel);
            state = IO_STATE_SET;
        } 
        else {
            io_reset(channel);
            state = IO_STATE_RESET;
        }
    }
    return state;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
io_state_t io_read(io_ch_t channel){
    io_state_t state = IO_STATE_RESET;
    if (channel < IO_CH_COUNT) {
        state = io_channel[channel];
    }
    return state;
}

// -----------------------------------------------------------------------------
// comm handler
// -----------------------------------------------------------------------------
void io_comm_handler(uint32_t rx_size){

}
