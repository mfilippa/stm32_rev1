// -----------------------------------------------------------------------------
// sch.h - MPF 11/2019
// -----------------------------------------------------------------------------


#ifndef _SCH_H_
#define _SCH_H_

// includes
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// definitions
#define SCH_NR_TIMERS    64        // max number of timers
#define SCH_NR_FUNCTIONS 64        // max nr of functions

// scheduled function typedef
typedef void (sch_function_t)(void);

// function handle
typedef uint32_t sch_handle_t;

// init: initializes module
void sch_init(void);

// tick: run this at systick, it generates a list of functions to be
// executed by sch_step()
void sch_tick(void);

// step: run this at background, it will execute scheduled functions when their
// time expires
void sch_step(void);

// function_register: register a function to be scheduled, reload in ticks
// returns the task handle
// a NULL function pointer disables the function
sch_handle_t sch_function_register(sch_function_t * function, uint32_t reload);

// function_set_reload: set ticks to execute scheduled function
void sch_function_set_reload(sch_handle_t handle, uint32_t reload);

// function_set: sets a function call to a specific handle
// a NULL function disables this task
void sch_function_set(sch_handle_t handle, sch_function_t * function);

// timer_register: registers a timer
// returns the timer handle
sch_handle_t sch_timer_register(void);

// timer_set: sets timer countdown in ticks
void sch_timer_set(sch_handle_t handle, uint32_t count);

// timer_has_expired: returns true if timer has expired, false otherwise
bool sch_timer_has_expired(sch_handle_t handle);

#endif // _SCH_H_
