// -----------------------------------------------------------------------------
// matlab wrapper - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "app/appinc.h"
#include "lib/libinc.h"
#include "hal/halinc.h"
#include "wrapper.h"

// definitions
#define WRAPPER_FREQUENCY	(10000)
#define WRAPPER_SLOW_COUNT		3		// WRAPPER_FREQUENCY/(1+N)
#define WRAPPER_SYSTICK_COUNT	9		// WRAPPER_FREQUENCY/(1+N)

// global matlab ports
const double * pin;
double * pout;

// module structure
struct wrapper_struct {
	uint32_t slow_count;
	uint32_t systick_count;
	uint32_t tick;
} wrapper;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void wrapper_init(const double * m_pin, double * m_pout){

    // get pointers to matlab data ports
    pin = m_pin;
    pout = m_pout;
    
    // wrapper init
    wrapper.slow_count = 0;
    wrapper.systick_count = 0;
    wrapper.tick = 0;

    // init
    app_init();
    
}

// -----------------------------------------------------------------------------
// step
// -----------------------------------------------------------------------------
void wrapper_step(void){

	// tick events
	if (wrapper.tick == 5.0*WRAPPER_FREQUENCY){

	}

	// fast process
	adc_fast_handler();

	// slow process
	if (wrapper.slow_count==0){
		adc_slow_handler();
		wrapper.slow_count = WRAPPER_SLOW_COUNT;
	}
	else {
		wrapper.slow_count--;
	}

	// systick process
	if (wrapper.systick_count==0){
		sys_tick_handler();
		wrapper.systick_count = WRAPPER_SYSTICK_COUNT;
	}
	else {
		wrapper.systick_count--;
	}

	// background process
	app_background();

	// increment tick
	wrapper.tick++;

}

