// -----------------------------------------------------------------------------
// uart.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _UART_H_
#define _UART_H_

// system includes
#include <stdint.h>

// init: initialize module, returns zero if successful
uint32_t uart_init(uint32_t baud_rate);

// write_ready: returns 1 if ready to write
uint32_t uart_write_ready(void);

// read_ready: returns 1 if ready to read
uint32_t uart_read_ready(void);

// write: sends a byte to uart
void uart_write(uint32_t data);

// read: reads a byte from uart
uint32_t uart_read(void);

#endif // _UART_H_
