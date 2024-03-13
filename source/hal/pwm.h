// -----------------------------------------------------------------------------
// pwm.h - Mariano F - 03/2024
// -----------------------------------------------------------------------------

#ifndef _PWM_H_
#define _PWM_H_

// public includes
#include <stdint.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// public defines
#define PWM_FREQ    (10000ul)

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init
void pwm_init(void);

// enable
void pwm_enable(void);

// disable
void pwm_disable(void);

// set duty cycle from 0-1
void pwm_set_duty(float * duty);

// comm handler
void pwm_comm_handler(uint32_t rx_size);

#endif // _PWM_H_
