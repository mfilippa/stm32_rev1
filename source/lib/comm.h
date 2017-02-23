// -----------------------------------------------------------------------------
// SerialComm 3.0 - MPF 01/2017
// -----------------------------------------------------------------------------


#ifndef _COMM_H_
#define _COMM_H_

// includes
#include <stdint.h>
#include "hal/uart.h"

// packet definitions
#define COMM_FRAME_START 		0x3A
#define COMM_FRAME_END			0x0A
#define COMM_FRAME_TIMEOUT		20

// comm handler typedef
typedef void (comm_handler_t)(uint32_t rx_size);

// init: initializes module. Buffers are passed to this module by pointers
// A handler is called after a message is received
// Requires UART access through HAL and scheduler SCH library
// Returns 0 if successful
uint32_t comm_init(uart_t uart, uint32_t buffer_size, uint8_t * rx_buffer,
		uint8_t * tx_buffer, comm_handler_t * handler);

// step: runs communications step, execute in background
void comm_step(void);

// read8: reads 1 byte from rx buffer, stops at end of buffer
uint32_t comm_read8(void);

// read16: reads 2 byte from rx buffer, stops at end of buffer
uint32_t comm_read16(void);

// read32: reads 4 byte from rx buffer, stops at end of buffer
uint32_t comm_read32(void);

// readf: reads a float (4 byte) from rx buffer, stops at end of buffer
float comm_readf(void);

// write8: writes 1 byte to tx buffer, stops at end of buffer
void comm_write8(uint32_t data);

// write16: writes 2 bytes to tx buffer, stops at end of buffer
void comm_write16(uint32_t data);

// write32: writes 4 bytes to tx buffer, stops at end of buffer
void comm_write32(uint32_t data);

// writef: writes a float (4 bytes) to tx buffer, stops at end of buffer
void comm_writef(float data);

#endif  // _COMM_H_

