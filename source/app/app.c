// -----------------------------------------------------------------------------
// app.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include <stdlib.h>
#include "app/app.h"
#include "lib/sch.h"
#include "lib/comm.h"
#include "hal/io.h"
#include "hal/sys.h"
#include "hal/uart.h"
#include "hal/pwm.h"
#include "hal/adc.h"
#include "lib/debug.h"
#ifdef MATLAB
#include "matlab/wrapper.h"
#endif

// defines
#define SYSTICK_FREQ_HZ     (1000)
#define COMM_BUFFER_SIZE    (10000)
#define DEBUG_BUFFER_SIZE   (1000)

// module structure
struct {
    // adc configuration
    adc_config_t adc_config;
    // comm buffer
    uint8_t rx_buffer[COMM_BUFFER_SIZE];
    uint8_t tx_buffer[COMM_BUFFER_SIZE];
    // debug buffer
    debug_t debug_buffer[DEBUG_BUFFER_SIZE];
} app;

// prototypes
void app_led_blink(void);
void app_comm_handler(uint32_t rx_size);
void app_systick(void);
void app_adc_trigger(void);
void app_adc_process_fast(void);
void app_adc_process_slow(void);

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
    error |= io_init();

    // init pwm
    error |= pwm_init(NULL);

    // init adc
    app.adc_config.fast_channel[0][0]=0;    // ADC0
    app.adc_config.fast_count[0]=1;         // one fast conversion
    app.adc_config.fast_count[1]=0;
    app.adc_config.fast_count[2]=0;
    app.adc_config.fast_pwm_trigger = 1;
    app.adc_config.slow_count = 0;
    app.adc_config.slow_pwm_trigger = 0;
    error |= adc_init(&app.adc_config, NULL, &app_adc_process_fast);

    // init uart
    error |= uart_init(UART0, UART_BAUD_115200);

    // init scheduler
    error |= sch_init();
    handle = sch_function_register(&app_led_blink,500);
    if (handle==0) error = 1;

    // init comm
    error |= comm_init(UART0, COMM_BUFFER_SIZE, app.rx_buffer, app.tx_buffer,
        &app_comm_handler);

    // init debug
    error |= debug_init(app.debug_buffer, DEBUG_BUFFER_SIZE);
    debug_wrap_off();
    debug_disable();

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
    io_toggle(IO_CH_LED);
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
    debug_save(adc_read(ADC_FAST_CH1));
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
        debug_enable();
        break;
    case 0x02:
        debug_clear();
        break;
    case 0x03: 
        for (uint32_t i=0; i<DEBUG_BUFFER_SIZE;i++) 
            comm_write32(app.debug_buffer[i]);
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
        comm_write16(adc_read(ADC_FAST_CH1));
        break;
    case 0xD3:
        break;
    default:
        // command not recognized
        comm_write8(0xFF);
    }
}

