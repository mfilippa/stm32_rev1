// -----------------------------------------------------------------------------
// i2c.c - MPF - 10/2013
// -----------------------------------------------------------------------------

// includes
#include "hal/halinc.h"
#include "wrapper.h"

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t i2c_init(void){
    return 0;
}

// -----------------------------------------------------------------------------
// write
// -----------------------------------------------------------------------------
void i2c_write(uint32_t address, uint32_t * data, uint32_t nr_bytes,
        void (*callback)(void)){
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
void i2c_read(uint32_t address, uint32_t * data, uint32_t nr_bytes,
        void (*callback)(void)){

}

// -----------------------------------------------------------------------------
// get state
// -----------------------------------------------------------------------------
uint32_t i2c_get_state(void){
    return 0;
}

// -----------------------------------------------------------------------------
// step
// -----------------------------------------------------------------------------
void i2c_step(void){

}

