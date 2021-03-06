// -----------------------------------------------------------------------------
// app.c - MPF 04/2020
// -----------------------------------------------------------------------------

// includes
#include "app.h"
#include "io.h"
#include "sys.h"
#include "uart.h"
#include "sch.h"
#include "comm.h"

// defines
#define SYSTICK_FREQ_HZ     (1000)
#define COMM_BUFFER_SIZE    (1000)

// module structure
struct {
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
void app_init(void) {

    // disable all interrupts
    sys_disable_interrupts();

    // init systick
    sys_tick_init(SYSTICK_FREQ_HZ, &app_systick);

    // init peripherals
    io_init();
    uart_init(UART_COMM,UART_BAUD_115200);

    // init scheduler
    sch_init();
    sch_function_register(&app_led_blink,500);

    // init comm
    comm_init(UART_COMM, COMM_BUFFER_SIZE, app.rx_buffer, app.tx_buffer,
        &app_comm_handler);

    // enable interrupts
    sys_enable_interrupts();

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

