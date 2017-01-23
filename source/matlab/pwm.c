// -----------------------------------------------------------------------------
// pwm.h - MPF - 2/2014
// -----------------------------------------------------------------------------

// includes
#include "hal/halinc.h"
#include "wrapper.h"

// definitions
#define PWM_FREQUENCY (10000)

// module structure
struct pwm_struct {
	void (*fault_handler)(void);
	uint32_t enabled;
	double value1;
	double value2;
	double value3;
} pwm;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t pwm_init(void (*fault_handler)(void)){
	// init
	pwm.enabled = 0;
	pout[0] = pout[1] = pout[2] = 0;
	pwm.value1 = pwm.value2 = pwm.value3 = 0.5;
	// store handler
	pwm.fault_handler = fault_handler;
	return 0;

}

// -----------------------------------------------------------------------------
// enable
// -----------------------------------------------------------------------------
void pwm_enable(void){
	pwm.enabled = 1;
	pout[0] = pwm.value1;
	pout[1] = pwm.value2;
	pout[2] = pwm.value3;
}

// -----------------------------------------------------------------------------
// disable
// -----------------------------------------------------------------------------
void pwm_disable(void){
	pwm.enabled = 0;
	pout[0] = pout[1] = pout[2] = 0;
}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void pwm_setq(uint32_t * amp){
	if(amp[1]>(1.0*(1<<14))) return;
	if(amp[2]>(1.0*(1<<14))) return;
	if(amp[3]>(1.0*(1<<14))) return;
	pwm.value1 = ((double)amp[1])/((double)(1<<14));
	pwm.value2 = ((double)amp[2])/((double)(1<<14));
	pwm.value3 = ((double)amp[3])/((double)(1<<14));
	if (pwm.enabled==1){
		pout[0] = pwm.value1;
		pout[1] = pwm.value2;
		pout[2] = pwm.value3;
	}
}

// -----------------------------------------------------------------------------
// simulated pwm fault
// -----------------------------------------------------------------------------
void pwm_matlab_fault(void){

	// call handler
	if(pwm.fault_handler!= 0) pwm.fault_handler();
}

