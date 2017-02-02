// -----------------------------------------------------------------------------
// app.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _APP_H_
#define _APP_H_

// system includes
#include <stdint.h>

// init: initializes application, returns 0 if successful
uint32_t app_init(void);

// background: background step
void app_background(void);

// systick: to be executed at system timer tick event
void app_systick(void);

// slow adc processing
void app_adc_process_slow(void);

// fast adc processing
void app_adc_process_fast(void);

#endif // _APP_H_
