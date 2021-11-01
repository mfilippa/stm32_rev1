// -----------------------------------------------------------------------------
// matlab wrapper - MPF 10/2021
// -----------------------------------------------------------------------------

// includes
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "sys.h"
#include "app.h"
#include "adc.h"
#include "pwm.h"
#include "io.h"
#include "wrapper.h"

// frequencies
#define FREQ_MAIN    (20000u)  // main freq in Hz
#define FREQ_FASTADC (20000u)  // fast adc in Hz
#define FREQ_SLOWADC  (5000u)  // slow adc in Hz
#define FREQ_SYSTICK  (1000u)  // systick in Hz
#define FREQ_DATAOUT     (1u)  // systick in Hz

// tick reload values - truncates to int, use fraction saving when truncating
#define RELOAD_SYSTICK  (FREQ_MAIN/FREQ_SYSTICK)
#define RELOAD_FASTADC  (FREQ_MAIN/FREQ_FASTADC)
#define RELOAD_SLOWADC  (FREQ_MAIN/FREQ_SLOWADC)
#define RELOAD_DATAOUT  (FREQ_MAIN/FREQ_DATAOUT)

// time to ticks
#define SIM_TIME_TO_TICKS(t)  ((uint64_t)((double)FREQ_MAIN*(double)(t)))

// global matlab ports
const double * pin;
double * pout;

// tick struct
struct tick_struct {
    uint64_t count;
    uint64_t fast_adc;
    uint64_t slow_adc;
    uint64_t systick;
    uint64_t dataout;
} tick;

// prototypes
void error_handler(uint32_t err);

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void wrapper_init(const double * m_pin, double * m_pout){

    // initialize vars
    tick.count = tick.systick = tick.fast_adc = tick.slow_adc = 
        tick.dataout = 0;

    // enable error checking
    error_init(&error_handler);
	
    // get pointers to matlab data ports
    pin = m_pin;
    pout = m_pout;
    
    // initialize app
    app_init();

}

// -----------------------------------------------------------------------------
// step
// -----------------------------------------------------------------------------
void wrapper_step(void){

    // input data here -----------------------------------------------------
    adc_data.fast_raw[ADC_FAST_CH0] = pin[0];
    adc_data.fast_raw[ADC_FAST_CH1] = pin[1];
    adc_data.fast_raw[ADC_FAST_CH2] = pin[2];
    adc_data.fast_raw[ADC_FAST_CH3] = pin[3];
    adc_data.slow_raw[ADC_SLOW_CH0] = pin[4];
    adc_data.slow_raw[ADC_SLOW_CH1] = pin[5];
    adc_data.slow_raw[ADC_SLOW_CH2] = pin[6];
    adc_data.slow_raw[ADC_SLOW_CH3] = pin[7];

    // run processes -------------------------------------------------------

    // background
    app_background();

    // run fast adc
    if (tick.fast_adc==0){
        if (adc_data.fast_handler!=0) adc_data.fast_handler();
        tick.fast_adc = RELOAD_FASTADC-1;
    }
    else tick.fast_adc--;

    // run slow adc
    if (tick.slow_adc==0){
        if (adc_data.slow_handler!=0) adc_data.slow_handler();
        tick.slow_adc = RELOAD_SLOWADC-1;
    }
    else tick.slow_adc--;

    // run systick
    if (tick.systick==0){
        if (sys_data.systick_handler!=0) sys_data.systick_handler();
        tick.systick = RELOAD_SYSTICK-1;
    }
    else tick.systick--;

    // output data here ----------------------------------------------------
    pout[0] = pwm_data.duty[0];
    pout[1] = pwm_data.duty[1];
    pout[2] = pwm_data.duty[2];
    pout[24] = io_data.state[IO_CH_LED];
    pout[25] = io_data.state[IO_CH_DEBUG1];
    pout[26] = io_data.state[IO_CH_DEBUG2];
    pout[27] = 0;
    pout[28] = 0;
    pout[29] = 0;

}

// -----------------------------------------------------------------------------
// error handler
// -----------------------------------------------------------------------------
void error_handler(uint32_t err){
    // do nothing
}

