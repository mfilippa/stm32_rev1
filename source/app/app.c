// -----------------------------------------------------------------------------
// app.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "app/app.h"
#include "lib/sch.h"
#include "lib/comm.h"
#include "hal/io.h"
#include "hal/sys.h"
#include "hal/uart.h"
#include "hal/halconfig.h"
#ifdef MATLAB
#include "matlab/wrapper.h"
#endif

// defines
#define SYSTICK_FREQ_HZ     (1000)
#define COMM_BUFFER_SIZE    (1000)

// module structure
struct {
    // io configuration
    io_config_t io[IO_CH_COUNT];
    // comm buffer
    uint8_t rx_buffer[COMM_BUFFER_SIZE];
    uint8_t tx_buffer[COMM_BUFFER_SIZE];
} app;

// prototypes
void app_led_blink(void);
void app_comm_handler(uint32_t rx_size);
void app_systick(void);
void app_adc_trigger(void);

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t app_init(void) {
    uint32_t error = 0;
    uint32_t handle = 0;

    // disable all interrupts
    sys_disable_interrupts();

    // init systick
    error |= sys_tick_init(SYSTICK_FREQ_HZ, &app_systick);

    // init gpio
    app.io[IO_LED].port = 0; 
    app.io[IO_LED].pin = 0; 
    app.io[IO_LED].type = 1; 
    app.io[IO_LED].ah = 1; 
    app.io[IO_LED].state = 0;
    app.io[IO_DEBUG].port = 0; 
    app.io[IO_DEBUG].pin = 1; 
    app.io[IO_DEBUG].type = 1; 
    app.io[IO_DEBUG].ah = 1; 
    app.io[IO_DEBUG].state = 0;
    error |= io_init(app.io, IO_CH_COUNT);

    // init scheduler
    error |= sch_init();
    handle = sch_function_register(&app_led_blink,500);
    if (handle==0) error = 1;

    // init comm
    error |= comm_init(UART0, COMM_BUFFER_SIZE, app.rx_buffer, app.tx_buffer,
        &app_comm_handler);

    // if no errors, enable interrupts
    if (!error) sys_enable_interrupts();

    return error;
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
    io_toggle(IO_LED);
}

// -----------------------------------------------------------------------------
// adc trigger
// -----------------------------------------------------------------------------
void app_adc_trigger(void) {
    // trigger fast conversion
    // adc_sw_trigger_fast();
    // trigger slow conversion
    // adc_sw_trigger_slow();
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

