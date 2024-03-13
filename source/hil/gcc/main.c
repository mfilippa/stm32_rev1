// -----------------------------------------------------------------------------
// main.c - MPF 10/2021
// -----------------------------------------------------------------------------

// includes
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "adc.h"
#include "dac.h"
#include "i2c.h"
#include "app.h"
#include "error.h"
#include "io.h"
#include "pwm.h"
#include "sys.h"
#include "uart.h"

// simulation variables
double gcc_pin[30];
double gcc_pout[30];
double * pin;
double * pout;

// frequencies
#define FREQ_MAIN    (PWM_FREQ)  // main freq in Hz
#define FREQ_TICK    (SYS_FREQ)  // sys tick in Hz
#define FREQ_DATAOUT   (100u)    // freq to send data out in Hz

// tick reload values
#define RELOAD_TICK  ((double)FREQ_MAIN/(double)FREQ_TICK)
#define RELOAD_TICK_FRACTION  (RELOAD_TICK - (double)((uint64_t)RELOAD_TICK))
#define RELOAD_DATAOUT   ((double)FREQ_MAIN/(double)FREQ_DATAOUT)
#define RELOAD_DATAOUT_FRACTION (RELOAD_DATAOUT - (double)((uint64_t)RELOAD_DATAOUT))

// simulation end
#define SIM_END_TIME   (10.0)  // end time in seconds
#define SIM_END_TICK   ((uint64_t)(((double)FREQ_MAIN)*(SIM_END_TIME)))

// time to ticks
#define SIM_TIME_TO_TICKS(t)  ((uint64_t)((double)FREQ_MAIN*(double)(t)))
#define SIM_TIME ((double)tick.count/(double)FREQ_MAIN)

// tick struct
struct tick_struct {
    uint64_t count;
    uint64_t sys;
    double sys_fraction;
    uint64_t dataout;
    double dataout_fraction;
} tick;

// simulated hardware
extern io_state_t io_channel[IO_CH_COUNT];
extern uint32_t adc_raw[ADC_CH_COUNT];
extern float pwm_duty;
extern bool pwm_enabled;

// file output
FILE * pfile;

// prototypes
void error_handler(uint32_t err);

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
int main(void) {

    // open file for output
    pfile = fopen("output.csv","w");
    if (pfile==NULL){
      printf("Cannot open output csv file");
      return 1;
    }

    printf("Simulation started\n");

    // init vars
    pin = gcc_pin;
    pout = gcc_pout;

    // enable error checking
    error_init(&error_handler);

    // init simulated hardware
    adc_init();
    //can_init();   XXX
    dac_init();
    i2c_init();
    io_init();
    pwm_init();
    //spi_init();   XXX
    uart_init();
    
    // initialize app
    app_init();

    // run processes
    for (tick.count=0;tick.count<SIM_END_TICK-1;tick.count++){

        // input data here -----------------------------------------------------
        

        // time data in
        if (tick.count == SIM_TIME_TO_TICKS(0.5)) {
        
        }

        // run processes -------------------------------------------------------

        // background
        app_background();

        // run system tick
        if (tick.sys==0){
            app_tick();
            tick.sys_fraction += RELOAD_TICK_FRACTION;
            tick.sys = ((uint64_t)RELOAD_TICK)-1+((uint64_t)tick.sys_fraction);
            tick.sys_fraction -= (double)((uint64_t)tick.sys_fraction);
        }
        else tick.sys--;

        // output data here ----------------------------------------------------

        // time data out
        if (tick.count == SIM_TIME_TO_TICKS(0.5)) {
            // time data out here
        }

        // periodic data out - print to CSV file
        if (tick.dataout==0){
            fprintf(pfile,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,\n",
                (double)tick.count/(double)FREQ_MAIN, 
                (double) io_channel[IO_CH_LED],
                (double) 0,
                (double) 0,
                (double) 0,
                (double) 0,
                (double) 0,
                (double) pout[27],
                (double) pout[28],
                (double) pout[29]
                );
            tick.dataout_fraction += RELOAD_DATAOUT_FRACTION;
            tick.dataout = ((uint64_t)RELOAD_DATAOUT)-1+((uint64_t)tick.dataout_fraction);
            tick.dataout_fraction -= (double)((uint64_t)tick.dataout_fraction);
        }
        else tick.dataout--;

    }

    // exit
    printf("\nSimulation ended\n");

    // close file
    fclose(pfile);

    return 0;
}

// -----------------------------------------------------------------------------
// error handler
// -----------------------------------------------------------------------------
void error_handler(uint32_t err){
    // print error and exit immediately
    printf("ERROR HANDLER CALLED, ERROR CODE %u\n", err);
    exit(0);
}