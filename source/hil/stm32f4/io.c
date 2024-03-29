// -----------------------------------------------------------------------------
// io.c - MPF 10/2021
// -----------------------------------------------------------------------------

// includes
#include <stdbool.h>
#include "stm32f4xx.h"
#include "io.h"

// io config table
typedef struct io_config_struct {
    GPIO_TypeDef * port;        // port: GPIOA, GPIOB, GPIOC, GPIOD
    uint16_t pin;               // pin: GPIO_Pin_0..15
    GPIOMode_TypeDef mode;      // mode: GPIO_Mode_IN / GPIO_Mode_OUT
    bool active_high;           // active high: true/false
    io_state_t init_state;      // initial state: set/reset
} io_config_t;

// -----------------------------------------------------------------------------
// gpio configuration
io_config_t io_cfg[IO_CH_COUNT] = {
//   port,  pin ,        mode,          ah,   init_state
    {GPIOC, GPIO_Pin_13, GPIO_Mode_OUT, true, IO_STATE_RESET},  // IO_CH_LED
    {GPIOC, GPIO_Pin_14, GPIO_Mode_OUT, true, IO_STATE_RESET},  // IO_CH_DEBUG1
    {GPIOC, GPIO_Pin_15, GPIO_Mode_OUT, true, IO_STATE_RESET},  // IO_CH_DEBUG2
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
void io_init(void){

    GPIO_InitTypeDef gpio;
    uint32_t i;

    // enable all peripherals
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    // configure gpio
    for (i = 0; i < IO_CH_COUNT; ++i) {
        gpio.GPIO_Pin = io_cfg[i].pin;
        gpio.GPIO_Mode = io_cfg[i].mode;
        gpio.GPIO_OType = GPIO_OType_PP;
        gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
        gpio.GPIO_Speed = GPIO_High_Speed;
        // initial state
        if (io_cfg[i].init_state==IO_STATE_RESET) io_reset((io_ch_t)i);
        else io_set((io_ch_t)i);
        GPIO_Init(io_cfg[i].port, &gpio);
    }

}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void io_set(io_ch_t channel){
    // no error checking for speed
    if(io_cfg[channel].active_high) {
        io_cfg[channel].port->BSRRL = io_cfg[channel].pin;
    }
    else {
        io_cfg[channel].port->BSRRH = io_cfg[channel].pin;
    }
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(io_ch_t channel){
    // no error checking for speed
    if(io_cfg[channel].active_high){
        io_cfg[channel].port->BSRRH = io_cfg[channel].pin;
    } 
    else {
        io_cfg[channel].port->BSRRL = io_cfg[channel].pin;
    }
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
io_state_t io_toggle(io_ch_t channel){
    io_state_t state;
    // no error checking for speed
    if (((io_cfg[channel].port->ODR) & 
            io_cfg[channel].pin) != (uint32_t)Bit_RESET) {
        io_cfg[channel].port->BSRRH = io_cfg[channel].pin;
        if(io_cfg[channel].active_high) state = IO_STATE_RESET;
        else state = IO_STATE_SET;
    }
    else {
        io_cfg[channel].port->BSRRL = io_cfg[channel].pin;
        if(io_cfg[channel].active_high) state = IO_STATE_SET;
        else state = IO_STATE_RESET;
    }
    return state;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
io_state_t io_read(io_ch_t channel){
    io_state_t state;
    // no error checking for speed
    if ((io_cfg[channel].port->IDR & 
        io_cfg[channel].pin) != (uint32_t)Bit_RESET)  {
        if(io_cfg[channel].active_high) state = IO_STATE_SET;
        else state = IO_STATE_RESET;
    }
    else {
        if(io_cfg[channel].active_high) state = IO_STATE_RESET;
        else state = IO_STATE_SET;
    }
    return state;
}

