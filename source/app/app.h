// -----------------------------------------------------------------------------
// app.h - Mariano F - 03/2024
// -----------------------------------------------------------------------------

#ifndef _APP_H_
#define _APP_H_

// public includes
#include <stdint.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// public defines
#define APP_FW_VERSION    "v0.0.0"

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init
void app_init(void);

// background step
void app_background(void);

// scheduler tick
void app_tick(void);

// comm handler
void app_comm_handler(uint32_t rx_size);

#endif // _APP_H_
