// -----------------------------------------------------------------------------
// app.h - MPF 04/2020
// -----------------------------------------------------------------------------

#ifndef _APP_H_
#define _APP_H_

// includes
#include <stdint.h>
#include <stdio.h>


// init: initializes application
void app_init(void);

// background: background step
void app_background(void);

// systick: to be executed at system timer tick event
void app_systick(void);

#endif // _APP_H_
