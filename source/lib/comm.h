// -----------------------------------------------------------------------------
// SerialComm 3.0 - MPF 11/2019
// -----------------------------------------------------------------------------
// Hex coded ascii comm protocol. Message structure:
//   <START_CHAR><DATA...><END_CHAR>
// -----------------------------------------------------------------------------

#ifndef _COMM_H_
#define _COMM_H_

// includes
#include <stdint.h>
#include "uart.h"

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// packet definitions
#define COMM_BUFFER_SIZE         (1000)
#define COMM_FRAME_START         (0x3A)     // ':'
#define COMM_FRAME_END           (0x0A)     // '\n'
#define COMM_MAX_STRING_LEN      (100u)
#define COMM_TIMEOUT            (5000u)  	// scheduler time steps
#define COMM_INVALID_CMD         (0xFF)

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init
void comm_init(void);

// communications step, execute in background
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

// writes: writes null terminated string
void comm_writestr(const char * string);

#endif  // _COMM_H_

