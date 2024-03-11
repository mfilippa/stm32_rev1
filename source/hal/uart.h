// -----------------------------------------------------------------------------
// uart.h - Mariano F - 04/2024
// -----------------------------------------------------------------------------

#ifndef _UART_H_
#define _UART_H_

// public includes
#include <stdint.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init
void uart_init(void);

// write_ready
bool uart_write_ready(void);

// read_ready
bool uart_read_ready(void);

// write
void uart_write(uint32_t data);

// read
uint32_t uart_read(void);

#endif // _UART_H_
