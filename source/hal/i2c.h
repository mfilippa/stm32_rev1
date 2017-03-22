// -----------------------------------------------------------------------------
// i2c.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _I2C_H_
#define _I2C_H_

// includes
#include <stdint.h>

// init: initializes module, returns 0 if successful
uint32_t i2c_init(void);

// step: i2c state machine, run in background
void i2c_step(void);

// write: queues a write to i2c
void i2c_write(uint32_t address, uint32_t * data, uint32_t nr_bytes,
        void (*callback)(void));

// read: queues a read from i2c
void i2c_read(uint32_t address, uint32_t * data, uint32_t nr_bytes,
        void (*callback)(void));

// get state: gets controller state, zero is IDLE
uint32_t i2c_get_state(void);

#endif // _I2C_H_
