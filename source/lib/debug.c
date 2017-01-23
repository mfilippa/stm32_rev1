// -----------------------------------------------------------------------------
// debug.c - MPF 01/2017
// -----------------------------------------------------------------------------


// includes
#include "libinc.h"

// debug vars
uint32_t out1, out2, out3;
uint32_t inp1, inp2, inp3;

// module structure
struct debug_struct {
	debug_t * data;
	uint32_t tsize;
	uint32_t index;
	uint32_t enable;
	uint32_t wrap;
} debug;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t debug_init(debug_t * data, uint32_t tsize){
	uint32_t i;
	debug.data = data;
	debug.tsize = tsize;
	debug.index = 0;
	debug.enable = 0;
	debug.wrap = 0;
	// clear buffer
	for (i=0; i<debug.tsize; i++){
		debug.data[i] = 0;
	}
	// success
	return 0;
}

// -----------------------------------------------------------------------------
// save
// -----------------------------------------------------------------------------
void debug_save(debug_t data){
	if (debug.enable==1){
		if (debug.index<debug.tsize){
			debug.data[debug.index] = data;
			debug.index++;
		}
		else if (debug.wrap==1){
			debug.index = 0;
			debug.data[debug.index] = data;
			debug.index++;
		}
	}
}

// -----------------------------------------------------------------------------
// set trigger
// -----------------------------------------------------------------------------
void debug_enable(void){
	debug.enable = 1;
}
// -----------------------------------------------------------------------------
// reset trigger
// -----------------------------------------------------------------------------
void debug_disable(void){
	debug.enable = 0;
}
// -----------------------------------------------------------------------------
// wrap on
// -----------------------------------------------------------------------------
void debug_wrap_on(void){
	debug.wrap = 1;
}
// -----------------------------------------------------------------------------
// wrap off
// -----------------------------------------------------------------------------
void debug_wrap_off(void){
	debug.wrap = 0;
}
// -----------------------------------------------------------------------------
// clear all
// -----------------------------------------------------------------------------
void debug_clear(void){
	uint32_t i;
	debug.enable = 0;
	debug.index = 0;
	// clear buffer
	for (i=0; i<debug.tsize; i++){
		debug.data[i] = 0;
	}
}

// -----------------------------------------------------------------------------
// is enabled
// -----------------------------------------------------------------------------
uint32_t debug_is_enabled(void){
	return debug.enable;
}
