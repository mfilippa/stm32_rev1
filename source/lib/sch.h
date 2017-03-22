// -----------------------------------------------------------------------------
// sch.h - MPF 01/2013
// -----------------------------------------------------------------------------


#ifndef _SCH_H_
#define _SCH_H_

// includes
#include <stdint.h>

// definitions
#define SCH_NR_TIMERS    64        // max number of timers
#define SCH_NR_FUNCTIONS 64        // max nr of functions

// init: initializes module, returns zero if successful
uint32_t sch_init(void);

// tick: run this at systick, it generates a list of functions to be
// executed by sch_step()
void sch_tick(void);

// step: run this at background, it will execute scheduled functions when their
// time expires
void sch_step(void);

// function_register: register a function to be scheduled, reload in ticks
// returns a handle (non-zero) if successful, zero if unable to register function
// a reload of ZERO disables the function
uint32_t sch_function_register(void (*function)(void), uint32_t reload);

// function_set_reload: set ticks to execute scheduled function
// a reload of ZERO disables the function
void sch_function_set_reload(uint32_t handle, uint32_t reload);

// function_set: sets a function call to a specific handle
void sch_function_set(uint32_t handle, void (*function)(void));

// timer_register: registers a timer
// returns a non-zero handle if successful, zero if no timer is available
uint32_t sch_timer_register(void);

// timer_set: sets timer countdown in ticks
void sch_timer_set(uint32_t handle, uint32_t count);

// timer_has_expired: returns 1 if timer has expired, zero otherwise
uint32_t sch_timer_has_expired(uint32_t handle);

#endif // _SCH_H_
