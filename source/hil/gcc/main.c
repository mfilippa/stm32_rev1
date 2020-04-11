// -----------------------------------------------------------------------------
// main.c - MPF 11/2019
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

// frequencies
#define FREQ_MAIN    (20000u)  // main freq in Hz
#define FREQ_SYSTICK  (1000u)  // systick in Hz
#define FREQ_FASTADC (20000u)  // fast adc in Hz
#define FREQ_SLOWADC  (5000u)  // slow adc in Hz

// tick reload values - truncates to int, use fraction saving when truncating
#define RELOAD_SYSTICK  (FREQ_MAIN/FREQ_SYSTICK)
#define RELOAD_FASTADC  (FREQ_MAIN/FREQ_FASTADC)
#define RELOAD_SLOWADC  (FREQ_MAIN/FREQ_SLOWADC)

// simulation end
#define SIM_END_TIME   (10.0)  // end time in seconds
#define SIM_END_TICK   ((uint64_t)(((double)FREQ_MAIN)*(SIM_END_TIME)))


// io ports - replicates matlab ports
double pin[30];
double pout[30];

// module struct
struct main_struct {
    uint64_t tick;
    uint64_t tick_systick;
} mn;

// prototypes
void error_handler(uint32_t err);

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
int main(void) {
    printf("Simulation started\n");

    // initialize vars
    mn.tick = mn.tick_systick = 0;

    // enable error checking
    error_init(&error_handler);
    
    // initialize app
    app_init();

    // run processes
    for (mn.tick=0;mn.tick<SIM_END_TICK-1;mn.tick++){

        // run background process
        app_background();

        // run systick
        if (mn.tick_systick==0){
            app_systick();
            mn.tick_systick = RELOAD_SYSTICK-1;
        }
        else mn.tick_systick--;

    }

    // exit
    printf("\nSimulation ended\n");
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