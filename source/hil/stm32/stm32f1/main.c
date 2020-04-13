// -----------------------------------------------------------------------------
// main.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "main.h"
#include "sys.h"
#include "io.h"
#include "error.h"
#include "app.h"

// hw includes
#include "stm32f10x.h"

// prototypes
void error_handler(error_t err);

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
int main(void) {

    //clocks are configured in SystemInit() in startup file

    // set up error handling
    error_init(&error_handler);

    // set up interrupt priorities: 0 to (1<<__NVIC_PRIO_BITS)-1
    NVIC_SetPriorityGrouping(__NVIC_PRIO_BITS);
    NVIC_SetPriority(TIM1_BRK_IRQn, (1<<__NVIC_PRIO_BITS) - 4);
    NVIC_SetPriority(DMA1_Channel1_IRQn, (1<<__NVIC_PRIO_BITS) - 3);
    NVIC_SetPriority(ADC1_2_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
    NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    // initialize app
    app_init();

    // background step
    while (1) {
        app_background();
    }
}

// -----------------------------------------------------------------------------
// error handler
// -----------------------------------------------------------------------------
void error_handler(error_t err){
    // get micro stuck in a loop
    sys_disable_interrupts();
    io_set(IO_CH_LED);
    while(1);
}
