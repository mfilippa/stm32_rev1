// -----------------------------------------------------------------------------
// sys.h - MPF 11/2019
// -----------------------------------------------------------------------------

#ifndef _SYS_H_
#define _SYS_H_

// includes
#include <stdint.h>
#include <stdio.h>

// tick_init: initializes a system timer
// pass a handler to call when timer expires, pass NULL to disable
void sys_tick_init(uint32_t frequency, void (*systick_handler)(void));

// reset: performs a software reset
void sys_reset(void);

// tick get: returns system timer tick count
uint32_t sys_tick_get(void);

// global interrupt enable
void sys_enable_interrupts(void);

// global interrupt disable
void sys_disable_interrupts(void);

#endif // _SYS_H_
