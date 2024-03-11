// -----------------------------------------------------------------------------
// sys.h - Mariano F - 04/2023
// -----------------------------------------------------------------------------

#ifndef _SYS_H_
#define _SYS_H_

// includes
#include <stdint.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// system tick frequency
#define SYS_FREQ     	(1000ul)

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// reset
void sys_reset(void);

// get system tick count
uint32_t sys_tick_get(void);

// comm handler
void sys_comm_handler(uint32_t rx_size);

#endif // _SYS_H_
