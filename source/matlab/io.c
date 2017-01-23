// -----------------------------------------------------------------------------
// io.c - MPF - 10/2013
// -----------------------------------------------------------------------------

// includes
#include "hal/halinc.h"
#include "wrapper.h"

// module structure
struct io_struct {
	double * pin[IO_CHANNEL_COUNT];
} io;

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t io_init(void){

	// pin mapping
	io.pin[IO_LED] 	  =	&(pout[20]);
	io.pin[IO_DEBUG1] = &(pout[21]);
	io.pin[IO_DEBUG2] = &(pout[22]);

	// success
	return 0;
}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void io_set(io_channel_t channel){
	if (channel>=IO_CHANNEL_COUNT) return;
	if (io.pin[channel]==0) return;
	*(io.pin[channel]) = 1;
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(io_channel_t channel){
	if (channel>=IO_CHANNEL_COUNT) return;
	if (io.pin[channel]==0) return;
	*(io.pin[channel]) = 0;
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
uint32_t io_toggle(io_channel_t channel){
	if (channel>=IO_CHANNEL_COUNT) return 0;
	if (io.pin[channel]==0) return 0;
	if (*(io.pin[channel]) == 0) {
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
uint32_t io_read(io_channel_t channel){
	if (channel>=IO_CHANNEL_COUNT) return 0;
	if (io.pin[channel]==0) return 0;
	if (*(io.pin[channel]) == 0) {
		return 0;
	} else {
		return 1;
	}
}

