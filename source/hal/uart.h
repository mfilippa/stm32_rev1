// -----------------------------------------------------------------------------
// uart.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _UART_H_
#define _UART_H_

// system includes
#include <stdint.h>

// uart interfaces
typedef enum uart_enum {
    UART0 = 0,
    UART1,
    UART_COUNT
} uart_t;

// baud rates
typedef enum uart_baud_enum {
    UART_BAUD_9600=0,
    UART_BAUD_115200,
    UART_BAUD_COUNT,
} uart_baud_t;

// init: initialize module, returns zero if successful
// uart: picks a uart to initialize
// baud: baud rate
uint32_t uart_init(uart_t uart, uart_baud_t baud_rate);

// write_ready: returns 1 if ready to write
uint32_t uart_write_ready(uart_t uart);

// read_ready: returns 1 if ready to read
uint32_t uart_read_ready(uart_t uart);

// write: sends a byte to uart
void uart_write(uart_t uart, uint32_t data);

// read: reads a byte from uart
uint32_t uart_read(uart_t uart);

#endif // _UART_H_
