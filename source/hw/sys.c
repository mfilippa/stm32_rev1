// -----------------------------------------------------------------------------
// sys.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "stm32f4xx.h"
#include "hal/sys.h"

// module structure
struct sys_struct {
	void (*handler)(void);
} sys;

// -----------------------------------------------------------------------------
// systick init
// -----------------------------------------------------------------------------
int32_t sys_tick_init(uint32_t frequency, void (*handler)(void)){

	// range check frequency
	if ((frequency<1)||(frequency>1000000)) return 1;
	// configure systick
	SysTick_Config(SystemCoreClock / frequency);
	// register handler
	sys.handler = handler;
	// return with no error
	return 0;
}

// -----------------------------------------------------------------------------
// reset
// -----------------------------------------------------------------------------
void sys_reset(void){
	NVIC_SystemReset();
}

// -----------------------------------------------------------------------------
// sys tick handler
// -----------------------------------------------------------------------------
void sys_tick_handler(void){
	// call handler
	if (sys.handler!=0) sys.handler();
}

// -----------------------------------------------------------------------------
// systick handler
// -----------------------------------------------------------------------------
void SysTick_Handler(void){
	sys_tick_handler();
}

// -----------------------------------------------------------------------------
// tick get
// -----------------------------------------------------------------------------
uint32_t sys_tick_get(void){
	return SysTick->VAL;
}


// -----------------------------------------------------------------------------
// enable interrupts
// -----------------------------------------------------------------------------
void sys_enable_interrupts(void){
	__enable_irq();
}

// -----------------------------------------------------------------------------
// disable interrupts
// -----------------------------------------------------------------------------
void sys_disable_interrupts(void){
	__disable_irq();
}




