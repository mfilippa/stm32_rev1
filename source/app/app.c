// -----------------------------------------------------------------------------
// app.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "app/app.h"
#include "lib/sch.h"
#include "lib/comm.h"
#include "hal/io.h"
#include "hal/sys.h"
#include "hal/adc.h"
#ifdef MATLAB
#include "matlab/wrapper.h"
#endif

// prototypes
void app_led_blink(void);
void app_comm_handler(uint32_t rx_size);


// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t app_init(void) {

    // success
    return 0;
}

// -----------------------------------------------------------------------------
// background
// -----------------------------------------------------------------------------
void app_background(void) {
    // comm step
    comm_step();
    // run scheduled tasks
    sch_step();
}

// -----------------------------------------------------------------------------
// systick
// -----------------------------------------------------------------------------
void app_systick(void) {
    // scheduler tick
    sch_tick();
}

// -----------------------------------------------------------------------------
// led blink
// -----------------------------------------------------------------------------
void app_led_blink(void) {
    io_toggle(0);
}

// -----------------------------------------------------------------------------
// dummy
// -----------------------------------------------------------------------------
void app_dummy(void) {

}
// -----------------------------------------------------------------------------
// adc trigger
// -----------------------------------------------------------------------------
void app_adc_trigger(void) {
    // trigger fast conversion
    adc_sw_trigger_fast();
    // trigger slow conversion
    adc_sw_trigger_slow();
}

// -----------------------------------------------------------------------------
// slow adc
// -----------------------------------------------------------------------------
void app_adc_process_slow(void) {

}

// -----------------------------------------------------------------------------
// fast adc
// -----------------------------------------------------------------------------
void app_adc_process_fast(void) {

}

// -----------------------------------------------------------------------------
// comm handler
// -----------------------------------------------------------------------------
void app_comm_handler(uint32_t rx_size) {

    uint32_t cmd, temp;

    // first byte is cmd, echo it
    cmd = comm_read8();
    comm_write8(cmd);

    // comm machine
    switch (cmd) {

    case 0x00:
        // echo
        for (temp = 1; temp < rx_size; temp++) {
            comm_write8(comm_read8());
        }
        break;
    case 0x01:
        break;
    case 0x02:
        break;
    case 0x03:
        break;
    case 0x04:
        break;
    case 0x05:
        break;
    case 0x06:
        break;
    case 0x07:
        break;
    case 0xD0:
        break;
    case 0xD1:
        break;
    case 0xD2:
        break;
    case 0xD3:
        break;
    default:
        // command not recognized
        comm_write8(0xFF);
    }
}

