// -----------------------------------------------------------------------------
// pwm.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _PWM_H_
#define _PWM_H_

// system includes
#include <stdint.h>

// init: initializes module, return 0 if successful
// pass a handler to call in case of PWM hardware fault, or ZERO to disable
// this function
uint32_t pwm_init(void (*fault_handler)(void));

// enable: enables the PWM outputs
void pwm_enable(void);

// disable: disables the PWM outputs
void pwm_disable(void);

// set: sets duty cycle from 0-1 in q14,
// if more than one value is needed, pass an array of amplitude values
void pwm_set(uint32_t * amp);

#endif // _PWM_H_
