// -----------------------------------------------------------------------------
// sys.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "stm32f4xx.h"
#include "sys.h"
#include "error.h"

// module structure
struct sys_struct {
    void (*handler)(void);
} sys;

// -----------------------------------------------------------------------------
// systick init
// -----------------------------------------------------------------------------
void sys_tick_init(uint32_t frequency, void (*handler)(void)){

    // range check frequency
    if ((frequency<1)||(frequency>1000000)) error_raise(ERROR_SYS_INIT);
    // configure systick
    SysTick_Config(SystemCoreClock / frequency);
    // register handler
    sys.handler = handler;
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
    // F4 micro: clock = 168MHz
    // 1 tick = 1/168MHz
    // tick is systick downcounter, it will hit zero at systick event
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




