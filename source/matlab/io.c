// -----------------------------------------------------------------------------
// io.c - MPF - 10/2013
// -----------------------------------------------------------------------------

// includes
#include "hal/halinc.h"
#include "wrapper.h"

// module structure
struct io_struct {
    io_config_t * config;
    uint32_t tsize;
} io;

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t io_init(io_config_t * io_config, uint32_t tsize){

    // store data
    io.config = io_config;
    io.tsize = tsize;

    // success
    return 0;
}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void io_set(uint32_t channel){
    if (channel>=io.tsize) return;
    io.config[channel].state = 1;
    pout[io.config[channel].port] = 1;
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(uint32_t channel){
    if (channel>=io.tsize) return;
    io.config[channel].state = 0;
    pout[io.config[channel].port] = 0;
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
uint32_t io_toggle(uint32_t channel){
    if (channel>=io.tsize) return 0;
    if (io.config[channel].state == 0) {
        io_set(channel);
        return 1;
    } else {
        io_reset(channel);
        return 0;
    }
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
uint32_t io_read(uint32_t channel){
    if (channel>=io.tsize) return 0;
    if (pin[io.config[channel].port] == 0) {
        return 0;
    } else {
        return 1;
    }
}

