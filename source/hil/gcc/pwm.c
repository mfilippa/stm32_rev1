// -----------------------------------------------------------------------------
// pwm.h - MPF 11/2019
// -----------------------------------------------------------------------------

// includes
#include "error.h"
#include "pwm.h"

// definitions
#define PWM_CH_A_PWMS       1   // nr of PWMs in channel
#define PWM_CH_XYZ_PWMS     3   // nr of PWMs in channel

// module struct
struct pwm_struct {
    double freq[PWM_CH_COUNT];
    bool enable[PWM_CH_COUNT];
    // duties handled by channel
    double duty_a[PWM_CH_A_PWMS];
    double duty_xyz[PWM_CH_XYZ_PWMS];
} pwm;

// math macro
#define Q2D(v,q)    ((double)(v)/((double)((int32_t)1<<(int32_t)(q))))

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void pwm_init(void (*fault_handler)(void)){
    // initial values
    for (int i=0; i<PWM_CH_COUNT; i++) {
        pwm.freq[i] = 0;
        pwm.enable[i] = false;
    }
    for (int i=0; i<PWM_CH_A_PWMS; i++) pwm.duty_a[i] = 0;
    for (int i=0; i<PWM_CH_XYZ_PWMS; i++) pwm.duty_xyz[i] = 0;
}

// -----------------------------------------------------------------------------
// enable channel
// -----------------------------------------------------------------------------
void pwm_enable(pwm_ch_t channel){
    if (channel<PWM_CH_COUNT) pwm.enable[channel] = true;
    else error_raise(ERROR_PWM_ARG);
}

// -----------------------------------------------------------------------------
// disable channel
// -----------------------------------------------------------------------------
void pwm_disable_ch(pwm_ch_t channel){
    if (channel<PWM_CH_COUNT) pwm.enable[channel] = false;
    else error_raise(ERROR_PWM_ARG);
}

// -----------------------------------------------------------------------------
// set freq in Hz
// -----------------------------------------------------------------------------
void pwm_set_freq_q(pwm_ch_t channel, uint32_t freq){
    if (channel<PWM_CH_COUNT) pwm.freq[channel] = (double)(freq);
    else error_raise(ERROR_PWM_ARG);
}

// -----------------------------------------------------------------------------
// set duty in fixed point q14 [0-1]
// -----------------------------------------------------------------------------
void pwm_set_duty_q(pwm_ch_t channel, uint32_t * duty){
    if (channel<PWM_CH_COUNT) {
        switch (channel){
            case PWM_CH_A:
                for (int i=0; i<PWM_CH_A_PWMS; i++) {
                    if (duty[i]>(1<<14)) error_raise(ERROR_PWM_ARG);
                    else pwm.duty_a[i] = Q2D(duty[i],14);
                }
            break;
            case PWM_CH_XYZ:
                for (int i=0; i<PWM_CH_XYZ_PWMS; i++) {
                    if (duty[i]>(1<<14)) error_raise(ERROR_PWM_ARG);
                    else pwm.duty_xyz[i] = Q2D(duty[i],14);
                }
            break;
            case PWM_CH_COUNT:
            break;
        }
    }
    else error_raise(ERROR_PWM_ARG);
}

// -----------------------------------------------------------------------------
// set duty in float [0-1]
// -----------------------------------------------------------------------------
void pwm_set_duty_f(pwm_ch_t channel, float * duty){
    if (channel<PWM_CH_COUNT) {
        switch (channel){
            case PWM_CH_A:
                for (int i=0; i<PWM_CH_A_PWMS; i++) {
                    if ((duty[i]<0.0)||(duty[i]>1.0)) error_raise(ERROR_PWM_ARG);
                    else pwm.duty_a[i] = (double)duty[i];
                }
            break;
            case PWM_CH_XYZ:
                for (int i=0; i<PWM_CH_XYZ_PWMS; i++) {
                    if ((duty[i]<0.0)||(duty[i]>1.0)) error_raise(ERROR_PWM_ARG);
                    else pwm.duty_xyz[i] = (double)duty[i];
                }
            break;
            case PWM_CH_COUNT:
            break;
        }
    }
    else error_raise(ERROR_PWM_ARG);
}