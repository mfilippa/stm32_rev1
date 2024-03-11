// -----------------------------------------------------------------------------
// app.c - Mariano F - 03/2024
// -----------------------------------------------------------------------------

// private includes
#include "app.h"
#include "comm.h"
#include "io.h"
#include "sch.h"

// defines
#define APP_LED_BLINK       (500u)  // led blink

// simulation ports
#ifdef SIMULATION
extern const double pin[30];
extern double pout[30];
#endif

// prototypes
void app_led_blink(void);

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void app_init(void) {

    // init scheduler - initialize this before any other module
    // some modules might rely on an initialized scheduler
    sch_init();
    sch_function_register(&app_led_blink,500);

    // initialize comm
    comm_init();

}

// -----------------------------------------------------------------------------
// background
// -----------------------------------------------------------------------------
void app_background(void) {
    // run scheduled tasks
    sch_step();
    // run communications
    comm_step();
}

// -----------------------------------------------------------------------------
// scheduler tick
// -----------------------------------------------------------------------------
void app_tick(void){
    // run scheduler tick only
    sch_tick();
}

// -----------------------------------------------------------------------------
// led blink
// -----------------------------------------------------------------------------
void app_led_blink(void) {
    io_toggle(IO_CH_LED);
}

// -----------------------------------------------------------------------------
// comm handler
// -----------------------------------------------------------------------------
const char * const fw_version = APP_FW_VERSION;
void app_comm_handler(uint32_t rx_size) {

    // first byte is cmd, echo it
    cmd = comm_read8();
    comm_write8(cmd);

    // commands
    switch(cmd){
    case (0x00):    // echo
        for (uint32_t tmp = 1; tmp < rx_size; tmp++) {
            comm_write8(comm_read8());
        }
        break;
    case (0x01):    // fw version and other status/faults
        // firmware version always last (variable string length)
        comm_writestr(fw_version);
        break;
    default:
        // command not recognized
        comm_write8(COMM_INVALID_CMD);
    }
}

