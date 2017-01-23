// -----------------------------------------------------------------------------
// io.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _IO_H_
#define _IO_H_

// system includes
#include <stdint.h>

// channel mapping
typedef enum io_channel_enum {
	IO_LED = 0,			// LED
	IO_DEBUG1,			// DEBUG1
	IO_DEBUG2,			// DEBUG2
	IO_CHANNEL_COUNT,
} io_channel_t;

// init: initializes module, returns 0 if successful
uint32_t io_init(void);

// set: sets channel to ACTIVE state
void io_set(io_channel_t channel);

// reset: resets channel to INACTIVE state
void io_reset(io_channel_t channel);

// toggle: toggles channel, returns current state, ACTIVE as 1, INACTIVE as 0
uint32_t io_toggle(io_channel_t channel);

// read: reads channel, returns current state, ACTIVE as 1, INACTIVE as 0
uint32_t io_read(io_channel_t channel);

#endif // _IO_H_
