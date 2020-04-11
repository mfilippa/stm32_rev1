// -----------------------------------------------------------------------------
// uart.h - MPF 11/2019
// -----------------------------------------------------------------------------

#ifndef _UART_H_
#define _UART_H_

// includes
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// uart interfaces - do not use physical ifaces, use hw abstracted names
typedef enum uart_enum {
    UART_COMM = 0,
    UART_COUNT
} uart_t;

// baud rates
typedef enum uart_baud_enum {
    UART_BAUD_9600=0,
    UART_BAUD_115200,
    UART_BAUD_COUNT,
} uart_baud_t;

// init: initialize module, returns zero if successful
// uart: uart to initialize, baud: baud rate
void uart_init(uart_t uart, uart_baud_t baud_rate);

// write_ready: returns true if ready to write
bool uart_write_ready(uart_t uart);

// read_ready: returns true if ready to read
bool uart_read_ready(uart_t uart);

// write: sends a byte to uart
void uart_write(uart_t uart, uint32_t data);

// read: reads a byte from uart
uint32_t uart_read(uart_t uart);

#endif // _UART_H_
