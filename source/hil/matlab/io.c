// -----------------------------------------------------------------------------
// io.c - MPF - 10/2019
// -----------------------------------------------------------------------------

// includes
#include "hal/io.h"
#include "wrapper.h"

// matlab port map
uint32_t io_port_map[IO_CH_COUNT] = {
    0,      // IO_CH_LED
    1,      // IO_CH
};

// channel state
uint32_t io_channel_state[IO_CH_COUNT] = {0, 0};

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t io_init(void){
    // nothing to do
    return 0;
}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void io_set(io_ch_t channel){
    io_channel_state[channel] = pout[io_port_map[channel]] = 1;
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(io_ch_t channel){
    io_channel_state[channel] = pout[io_port_map[channel]] = 0;
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
io_state_t io_toggle(io_ch_t channel){
    if (io_channel_state[channel]) {
        io_set(channel);
        return IO_STATE_SET;
    } else {
        io_reset(channel);
        return IO_STATE_RESET;
    }
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
io_state_t io_read(io_ch_t channel){
    if (pin[io_port_map[channel]] == 0) {
        return IO_STATE_RESET;
    } else {
        return IO_STATE_SET;
    }
}

