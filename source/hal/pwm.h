// -----------------------------------------------------------------------------
// pwm.h - MPF 11/2019
// -----------------------------------------------------------------------------

#ifndef _PWM_H_
#define _PWM_H_

// includes
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// pwm channels 
// channels can represent one or multiple PWMs
// implementation is left to hardware layer
typedef enum pwm_ch_enum{
    PWM_CH_UVW,
    PWM_CH_COUNT,
} pwm_ch_t;

// init: initializes module
// pass a function to PWM hardware fault IRQ, or NULL to disable
void pwm_init(void (*fault_handler)(void));

// enable: enable channel
void pwm_enable(pwm_ch_t channel);

// disable: disable channel
void pwm_disable(pwm_ch_t channel);

// set: sets freq in Hz
void pwm_set_freq(pwm_ch_t channel, uint32_t freq);

// set: sets duty cycle from 0-1 in q14
// this function can set multiple duties (left to implementation layer)
void pwm_set_duty_q(pwm_ch_t channel, uint32_t * duty);

// set: sets duty cycle from 0-1 in float
// this function can set multiple duties (left to implementation layer)
void pwm_set_duty_f(pwm_ch_t channel, float * duty);

#endif // _PWM_H_
