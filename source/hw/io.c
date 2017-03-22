// -----------------------------------------------------------------------------
// io.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "stm32f4xx.h"
#include "hal/io.h"

// module structure
struct io_struct {
    io_config_t * config;
    uint32_t tsize;
} io;

// port mapping
GPIO_TypeDef * io_port_map[4] = { GPIOA, GPIOB, GPIOC, GPIOD };

// pin mapping
uint16_t io_pin_map[16] = { GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3,
    GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9,
    GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15,
};

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t io_init(io_config_t * io_config, uint32_t tsize){

    GPIO_InitTypeDef gpio;
    uint32_t i;

    // store data
    io.config = io_config;
    io.tsize = tsize;

    // range check
    for (i=0;i<io.tsize;i++){
        if (io.config[i].port>3) return 1;
        if (io.config[i].pin>15) return 1;
    }

    // enable all required peripheral clocks here -----------
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    // ------------------------------------------------------

    // configure pins
    for (i = 0; i < io.tsize; ++i) {
        gpio.GPIO_Pin = io_pin_map[io.config[i].pin];
        if (io.config[i].type==0) {
            // input
            gpio.GPIO_Mode = GPIO_Mode_IN;
        } else {
            // output
            gpio.GPIO_Mode = GPIO_Mode_OUT;
        }
        gpio.GPIO_OType = GPIO_OType_PP;
        gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
        gpio.GPIO_Speed = GPIO_High_Speed;
        // initial state
        if (io.config[i].state==0) io_reset(i);
        else io_set(i);
        GPIO_Init(io_port_map[io.config[i].port], &gpio);
    }

    // success
    return 0;
}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void io_set(uint32_t channel){
    if (channel<io.tsize) {
        if(io.config[channel].ah==0){
            io_port_map[io.config[channel].port]->BSRRH = io_pin_map[io.config[channel].pin];
        } else {
            io_port_map[io.config[channel].port]->BSRRL = io_pin_map[io.config[channel].pin];
        }
    }
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(uint32_t channel){
    if (channel<io.tsize) {
        if(io.config[channel].ah==0){
            io_port_map[io.config[channel].port]->BSRRL = io_pin_map[io.config[channel].pin];
        } else {
            io_port_map[io.config[channel].port]->BSRRH = io_pin_map[io.config[channel].pin];
        }
    }
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
uint32_t io_toggle(uint32_t channel){
    uint32_t state = 0;
    if (channel<io.tsize) {
        if (((io_port_map[io.config[channel].port]->ODR) & io_pin_map[io.config[channel].pin]) != (uint32_t)Bit_RESET) {
            io_port_map[io.config[channel].port]->BSRRH = io_pin_map[io.config[channel].pin];
            state = 0;
        }
        else {
            io_port_map[io.config[channel].port]->BSRRL = io_pin_map[io.config[channel].pin];
            state = 1;
        }
    }
    return state;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
uint32_t io_read(uint32_t channel){
    uint32_t state = 0;
    if (channel<io.tsize){
        if ((io_port_map[io.config[channel].port]->IDR & io_pin_map[io.config[channel].pin]) != (uint32_t)Bit_RESET)  {
            state = 1;
        }
        else {
            state = 0;
        }
    }
    return state;
}

