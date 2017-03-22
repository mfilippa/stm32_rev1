// -----------------------------------------------------------------------------
// main.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "main.h"
#include "hal/sys.h"
#include "app/app.h"

// hw includes
#include "stm32f4xx.h"

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
int main(void) {

    //clocks are configured in SystemInit() in startup file

    // disable interrupts
    sys_disable_interrupts();

    // set up interrupt priorities: 0 to (1<<__NVIC_PRIO_BITS)-1
    NVIC_SetPriorityGrouping(__NVIC_PRIO_BITS);
    NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, (1<<__NVIC_PRIO_BITS) - 4);
    NVIC_SetPriority(DMA2_Stream0_IRQn, (1<<__NVIC_PRIO_BITS) - 3);
    NVIC_SetPriority(ADC_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
    NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    // initialize app
    if (app_init()!=0) {
        // failed init, get micro stuck in a loop
        while(1);
    }

    // enable interrupts
    sys_enable_interrupts();

    // background step
    while (1) {
        app_background();
    }
}
