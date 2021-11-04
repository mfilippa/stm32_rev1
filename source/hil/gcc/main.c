// -----------------------------------------------------------------------------
// main.c - MPF 10/2021
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

// frequencies
#define FREQ_MAIN    (20000u)  // main freq in Hz
#define FREQ_FASTADC (20000u)  // fast adc in Hz
#define FREQ_SLOWADC  (5000u)  // slow adc in Hz
#define FREQ_SYSTICK  (1000u)  // systick in Hz
#define FREQ_DATAOUT   (100u)  // freq to send data out in Hz

// tick reload values - truncates to int, use fraction saving when truncating
#define RELOAD_SYSTICK  (FREQ_MAIN/FREQ_SYSTICK)
#define RELOAD_FASTADC  (FREQ_MAIN/FREQ_FASTADC)
#define RELOAD_SLOWADC  (FREQ_MAIN/FREQ_SLOWADC)
#define RELOAD_DATAOUT  (FREQ_MAIN/FREQ_DATAOUT)

// simulation end
#define SIM_END_TIME   (10.0)  // end time in seconds
#define SIM_END_TICK   ((uint64_t)(((double)FREQ_MAIN)*(SIM_END_TIME)))

// time to ticks
#define SIM_TIME_TO_TICKS(t)  ((uint64_t)((double)FREQ_MAIN*(double)(t)))

// io ports - replicates matlab ports
double pin[30];
double pout[30];

// tick struct
struct tick_struct {
    uint64_t count;
    uint64_t fast_adc;
    uint64_t slow_adc;
    uint64_t systick;
    uint64_t dataout;
} tick;

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

    // initialize vars
    tick.count = tick.systick = tick.fast_adc = tick.slow_adc = 
        tick.dataout = 0;

    // enable error checking
    error_init(&error_handler);
    
    // initialize app
    app_init();

    // run processes
    for (tick.count=0;tick.count<SIM_END_TICK-1;tick.count++){

        // input data here -----------------------------------------------------
        for(int i=0; i<ADC_FAST_COUNT; i++) adc_data.fast_raw[i] = 0;
        for(int i=0; i<ADC_SLOW_COUNT; i++) adc_data.slow_raw[i] = 0;

        // time data in
        if (tick.count == SIM_TIME_TO_TICKS(0.5)) {
            // time data in here
        }

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

        // periodic data out - print to CSV file
        if (tick.dataout==0){
            fprintf(pfile,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,\n",
                (double)tick.count/(double)FREQ_MAIN, 
                (double) io_data.state[IO_CH_LED],
                (double) io_data.state[IO_CH_DEBUG1],
                (double) io_data.state[IO_CH_DEBUG2],
                (double) 0,
                (double) 0,
                (double) 0,
                (double) 0,
                (double) 0,
                (double) 0
                );
            tick.dataout = RELOAD_DATAOUT-1;
        }
        else tick.dataout--;

        // time data out
        if (tick.count == SIM_TIME_TO_TICKS(0.5)) {
            // time data out here
        }

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
    printf("Error: %u\n", err);
    exit(0);
}