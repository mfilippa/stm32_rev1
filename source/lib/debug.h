// -----------------------------------------------------------------------------
// debug.h - MPF 01/2017
// -----------------------------------------------------------------------------


#ifndef _DEBUG_H_
#define _DEBUG_H_

// includes
#include <stdint.h>

// debug variables
extern uint32_t inp1, inp2, inp3;
extern uint32_t out1, out2, out3;

// data type definition
typedef uint32_t debug_t;

// init: initializes module, pass buffer of data and size of buffer
// returns 0 if successful
uint32_t debug_init(debug_t * data, uint32_t tsize);

// enable: enables saving data through debug_save()
void debug_enable(void);

// disable: disables saving data
void debug_disable(void);

// save: save data, advances through buffer automatically
// it wraps if wrap feature is enabled
void debug_save(debug_t data);

// wrap_on: enable buffer wrap (circular buffer)
void debug_wrap_on(void);

// wrap_off: disable buffer wrap, save will stop at end of buffer
void debug_wrap_off(void);

// clear: clears buffer contents
void debug_clear(void);

#endif //_DEBUG_H_
