// -----------------------------------------------------------------------------
// io.c - MPF 10/2019
// -----------------------------------------------------------------------------

// includes
#include <stdbool.h>
#include "stm32f4xx.h"
#include "hal/io.h"

// io config table
typedef struct io_config_struct {
    // port: GPIOA, GPIOB, GPIOC, GPIOD
    GPIO_TypeDef * port;
    // pin: GPIO_Pin_0..15
    uint16_t pin;
    // mode: GPIO_Mode_IN / GPIO_Mode_OUT
    GPIOMode_TypeDef mode;
    // active high: true for active high, false for active low
    bool active_high;
    // initial state: IO_STATE_SET / IO_STATE_RESET
    io_state_t init_state;
} io_config_t;

// pin configuration
io_config_t io_config[IO_CH_COUNT] = {
    // port, pin , mode, active_high, init_state
    GPIOC, GPIO_Pin_13, GPIO_Mode_OUT, true, IO_STATE_RESET,  // IO_CH_LED
    GPIOC, GPIO_Pin_14, GPIO_Mode_OUT, true, IO_STATE_RESET,  // IO_CH_DEBUG
};

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t io_init(void){

    GPIO_InitTypeDef gpio;
    uint32_t i;

    // enable all required peripheral
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    // configure pins
    for (i = 0; i < IO_CH_COUNT; ++i) {
        gpio.GPIO_Pin = io_config[i].pin;
        gpio.GPIO_Mode = io_config[i].mode;
        gpio.GPIO_OType = GPIO_OType_PP;
        gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
        gpio.GPIO_Speed = GPIO_High_Speed;
        // initial state
        if (io_config[i].init_state==IO_STATE_RESET) io_reset((io_ch_t)i);
        else io_set((io_ch_t)i);
        GPIO_Init(io_config[i].port, &gpio);
    }

    // success
    return 0;
}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void io_set(io_ch_t channel){
    if(io_config[channel].active_high){
        io_config[channel].port->BSRRL = io_config[channel].pin;
    } 
    else {
        io_config[channel].port->BSRRH = io_config[channel].pin;
    }
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(io_ch_t channel){
    if(io_config[channel].active_high){
        io_config[channel].port->BSRRH = io_config[channel].pin;
    } 
    else {
        io_config[channel].port->BSRRL = io_config[channel].pin;
    }
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
io_state_t io_toggle(io_ch_t channel){
    io_state_t state;
    if (((io_config[channel].port->ODR) & 
            io_config[channel].pin) != (uint32_t)Bit_RESET) {
        io_config[channel].port->BSRRH = io_config[channel].pin;
        if(io_config[channel].active_high) state = IO_STATE_RESET;
        else state = IO_STATE_SET;
    }
    else {
        io_config[channel].port->BSRRL = io_config[channel].pin;
        if(io_config[channel].active_high) state = IO_STATE_SET;
        else state = IO_STATE_RESET;
    }
    return state;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
io_state_t io_read(io_ch_t channel){
    io_state_t state;
    if ((io_config[channel].port->IDR & 
        io_config[channel].pin) != (uint32_t)Bit_RESET)  {
        if(io_config[channel].active_high) state = IO_STATE_SET;
        else state = IO_STATE_RESET;
    }
    else {
        if(io_config[channel].active_high) state = IO_STATE_RESET;
        else state = IO_STATE_SET;
    }
    return state;
}

