// -----------------------------------------------------------------------------
// pwm.h - MPF 10/2021
// -----------------------------------------------------------------------------
// 3 phase PWM module with BRK input
// -----------------------------------------------------------------------------

#ifndef _PWM_H_
#define _PWM_H_

// includes
#include <stdint.h>
#include <stdbool.h>

// *** for simulation environment only ***
// pwm data
#define PWM_NR_CHANNELS 3
typedef struct pwm_data_struct {
    double freq;
    double duty[PWM_NR_CHANNELS];
    bool enable;
} pwm_data_t;
extern pwm_data_t pwm_data;
// *** end simulation environment data ***

// init module
// pass a function to PWM hardware fault IRQ, or NULL to disable
// initializes with 0.5 duty to all channels
void pwm_init(uint32_t freq, uint32_t deadtime, void (*fault_handler)(void));

// set deadtime - see implementation layer on details on how this works
void pwm_set_deadtime(uint32_t deadtime);

// enable
void pwm_enable(void);

// disable channel
void pwm_disable(void);

// set freq in Hz - MUST FOLLOW with set_duty to update periods!
void pwm_set_freq(uint32_t freq);

// set duty cycle from 0-1 in q14
void pwm_set_duty_q(uint32_t * duty_q14);

// set duty cycle from 0-1 in float
void pwm_set_duty_f(float * duty);

#endif // _PWM_H_
