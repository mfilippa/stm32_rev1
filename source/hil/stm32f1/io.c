// -----------------------------------------------------------------------------
// io.c - MPF 11/2019
// -----------------------------------------------------------------------------

// includes
#include <stdbool.h>
#include "stm32f10x.h"
#include "error.h"
#include "io.h"

// io config table
typedef struct io_config_struct {
    GPIO_TypeDef * port;        // port: GPIOA, GPIOB, GPIOC, GPIOD
    uint16_t pin;               // pin: GPIO_Pin_0..15
    GPIOMode_TypeDef mode;      // mode: GPIO_Mode_IN_FLOATING/GPIO_Mode_Out_PP
    bool active_high;           // active high: true/false
    io_state_t init_state;      // initial state: set/reset
} io_config_t;

// -----------------------------------------------------------------------------
// gpio configuration
io_config_t io_cfg[IO_CH_COUNT] = {
//  port,  pin ,        mode,          ah,   init_state
    {GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP, false, IO_STATE_RESET},  // IO_CH_LED
    {GPIOC, GPIO_Pin_14, GPIO_Mode_Out_PP, true, IO_STATE_RESET},  // IO_CH_DEBUG
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
void io_init(void){

    GPIO_InitTypeDef gpio;
    uint32_t i;

    // check ports, pins and mode, the rest are typedef constrained
    for (i = 0; i < IO_CH_COUNT; ++i) {
        if ((io_cfg[i].port!=GPIOA)&&(io_cfg[i].port!=GPIOB)&&
            (io_cfg[i].port!=GPIOC)&&(io_cfg[i].port!=GPIOD)) 
            error_raise(ERROR_IO_INIT);
        if (!IS_GPIO_PIN(io_cfg[i].pin)) error_raise(ERROR_IO_INIT);
        if ((io_cfg[i].mode!=GPIO_Mode_IN_FLOATING)
            &&(io_cfg[i].mode!=GPIO_Mode_Out_PP))
            error_raise(ERROR_IO_INIT);
    }

    // enable all peripherals
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    // configure gpio
    for (i = 0; i < IO_CH_COUNT; ++i) {
        gpio.GPIO_Pin = io_cfg[i].pin;
        gpio.GPIO_Speed = GPIO_Speed_50MHz;
        gpio.GPIO_Mode = io_cfg[i].mode;
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
    if (channel<IO_CH_COUNT) {
        if(io_cfg[channel].active_high) {
            io_cfg[channel].port->BSRR = io_cfg[channel].pin;
        }
        else  {
            io_cfg[channel].port->BRR = io_cfg[channel].pin;
        }
    }
    else error_raise(ERROR_IO_ARG);
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(io_ch_t channel){
    if (channel<IO_CH_COUNT) {
        if(io_cfg[channel].active_high){
            io_cfg[channel].port->BRR = io_cfg[channel].pin;
        } 
        else {
            io_cfg[channel].port->BSRR = io_cfg[channel].pin;
        }
    }
    else error_raise(ERROR_IO_ARG);
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
io_state_t io_toggle(io_ch_t channel){
    io_state_t state;
    if (channel<IO_CH_COUNT) {
        if (((io_cfg[channel].port->ODR) & 
                io_cfg[channel].pin) != (uint32_t)Bit_RESET) {
            io_cfg[channel].port->BRR = io_cfg[channel].pin;
            if(io_cfg[channel].active_high) state = IO_STATE_RESET;
            else state = IO_STATE_SET;
        }
        else {
            io_cfg[channel].port->BSRR = io_cfg[channel].pin;
            if(io_cfg[channel].active_high) state = IO_STATE_SET;
            else state = IO_STATE_RESET;
        }
    }
    else error_raise(ERROR_IO_ARG);
    return state;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
io_state_t io_read(io_ch_t channel){
    io_state_t state;
    if (channel<IO_CH_COUNT) {
        if ((io_cfg[channel].port->IDR & 
            io_cfg[channel].pin) != (uint32_t)Bit_RESET)  {
            if(io_cfg[channel].active_high) state = IO_STATE_SET;
            else state = IO_STATE_RESET;
        }
        else {
            if(io_cfg[channel].active_high) state = IO_STATE_RESET;
            else state = IO_STATE_SET;
        }
    }
    else error_raise(ERROR_IO_ARG);
    return state;
}

