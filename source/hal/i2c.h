// -----------------------------------------------------------------------------
// i2c.h - MPF 11/2019
// -----------------------------------------------------------------------------

#ifndef _I2C_H_
#define _I2C_H_

// includes
#include <stdint.h>
#include <stdio.h>

// i2c states
typedef enum i2c_state_enum {
    I2C_STATE_IDLE,
    I2C_STATE_BUSY,
} i2c_state_t;

// init: initializes module
void i2c_init(void);

// step: i2c state machine, run in background
void i2c_step(void);

// write: queues a write to i2c
// pass a callback if required, NULL to disable
void i2c_write(uint32_t address, uint32_t * data, uint32_t nr_bytes,
        void (*callback)(void));

// read: queues a read from i2c
// pass a callback if required, NULL to disable
void i2c_read(uint32_t address, uint32_t * data, uint32_t nr_bytes,
        void (*callback)(void));

// get state: gets controller state
i2c_state_t i2c_get_state(void);

#endif // _I2C_H_
