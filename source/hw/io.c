// -----------------------------------------------------------------------------
// io.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "stm32f4xx.h"
#include "hal/io.h"

// pin typedef
typedef struct io_pin_struct{
	GPIO_TypeDef * port;		// pin port
	uint32_t pin_nr;			// pin nr
	GPIOMode_TypeDef mode;		// mode: OUT or IN
} io_pin_t;

// module structure
struct io_struct {
	io_pin_t pin[IO_CHANNEL_COUNT];
} io;


// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t io_init(void){

	GPIO_InitTypeDef gpio;
	uint32_t i;

	// enable all required peripheral clocks here -----------
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	// ------------------------------------------------------

	// configure pins here ----------------------------------
	// LED
	io.pin[IO_LED].port = GPIOC;
	io.pin[IO_LED].pin_nr = GPIO_Pin_13;
	io.pin[IO_LED].mode = GPIO_Mode_OUT;
	// DEBUG1
	io.pin[IO_DEBUG1].port = GPIOC;
	io.pin[IO_DEBUG1].pin_nr = GPIO_Pin_6;
	io.pin[IO_DEBUG1].mode = GPIO_Mode_OUT;
	// DEBUG2
	io.pin[IO_DEBUG2].port = GPIOC;
	io.pin[IO_DEBUG2].pin_nr = GPIO_Pin_7;
	io.pin[IO_DEBUG2].mode = GPIO_Mode_OUT;
	// ------------------------------------------------------

	// configure pins
	for (i = 0; i < IO_CHANNEL_COUNT; ++i) {
		gpio.GPIO_Pin = io.pin[i].pin_nr;
		gpio.GPIO_Mode = io.pin[i].mode;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
		gpio.GPIO_Speed = GPIO_High_Speed;
		GPIO_Init(io.pin[i].port, &gpio);
	}

	// success
	return 0;
}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void io_set(io_channel_t channel){
	if (channel<IO_CHANNEL_COUNT) {
		io.pin[channel].port->BSRRL = io.pin[channel].pin_nr;
	}
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void io_reset(io_channel_t channel){
	if (channel<IO_CHANNEL_COUNT) {
		io.pin[channel].port->BSRRH = io.pin[channel].pin_nr;
	}
}

// -----------------------------------------------------------------------------
// toggle
// -----------------------------------------------------------------------------
uint32_t io_toggle(io_channel_t channel){
	uint32_t state = 0;
	if (channel<IO_CHANNEL_COUNT) {
		if (((io.pin[channel].port->ODR) & io.pin[channel].pin_nr) != (uint32_t)Bit_RESET) {
			io.pin[channel].port->BSRRH = io.pin[channel].pin_nr;
			state = 0;
		}
		else {
			io.pin[channel].port->BSRRL = io.pin[channel].pin_nr;
			state = 1;
		}
	}
	return state;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
uint32_t io_read(io_channel_t channel){
	uint32_t state = 0;
	if (channel<IO_CHANNEL_COUNT){
		if ((io.pin[channel].port->IDR & io.pin[channel].pin_nr) != (uint32_t)Bit_RESET)  {
			state = 1;
		}
		else {
			state = 0;
		}
	}
	return state;
}

