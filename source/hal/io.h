// -----------------------------------------------------------------------------
// io.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _IO_H_
#define _IO_H_

// system includes
#include <stdint.h>

// io config table
typedef struct io_config_struct {
    // port: 0-3 for Port A to Port D
    uint32_t port;
    // pin: 0-15
    uint32_t pin;
    // type: 1 for output, 0 for input
    uint32_t type;
    // active high: 1 for active high, 0 for active low
    uint32_t ah;
    // initial state: 1 for set, 0 for reset
    uint32_t state;
} io_config_t;

// init: initializes module, returns 0 if successful
uint32_t io_init(io_config_t * io_config, uint32_t tsize);

// set: sets channel to ACTIVE state
void io_set(uint32_t channel);

// reset: resets channel to INACTIVE state
void io_reset(uint32_t channel);

// toggle: toggles channel, returns current state, ACTIVE as 1, INACTIVE as 0
uint32_t io_toggle(uint32_t channel);

// read: reads channel, returns current state, ACTIVE as 1, INACTIVE as 0
uint32_t io_read(uint32_t channel);

#endif // _IO_H_
