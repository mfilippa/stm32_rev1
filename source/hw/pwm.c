// -----------------------------------------------------------------------------
// pwm.h - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "stm32f4xx.h"
#include "hal/pwm.h"
#include "lib/macros.h"

// PWM definitions
#define PWM_FREQUENCY (12000)
#define PWM_PERIOD    (SystemCoreClock/PWM_FREQUENCY/2)
#define PWM_DEADTIME    (148)    // see reference manual on value calculation

// comment the following line to map amplitude to CHx
#define PWM_AMP_TO_CHxN

// comment the following line for active low
#define PWM_CHx_ACTIVE_HIGH
#define PWM_CHxN_ACTIVE_HIGH
#define PWM_BREAK_ACTIVE_HIGH

// slow ADC trigger frequency
#define PWM_ADC_DIV        2        // ADC_FREQ = PWM_FREQUENCY/(2+PWM_ADC_DIV)

// comment this line to adjust ADC trigger alignment to PWM
//#define PWM_ADC_TRIGER_SHIFT

// module structure
struct pwm_struct {
    void (*fault_handler)(void);
} pwm;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t pwm_init(void (*fault_handler)(void)){

    // configuration:
    //    PA8        TMR1_CH1    HIN_A
    //    PB13    TMR1_CH1N    /LIN_A (active low)
    //    PA9        TMR1_CH2    HIN_B
    //    PB14    TMR1_CH2N    /LIN_B (active low)
    //    PA10    TMR1_CH3    HIN_C
    //    PB15    TMR1_CH3N    /LIN_C (active low)
    //    PB12    TMR1_BKIN    SD/OC  (active low)
    //  TMR2 for ADC timing

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

    // enable clocks to peripherals
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // GPIO Configuration
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // break input

    // AF configuration
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource12, GPIO_AF_TIM1);        // break input

    // Time base configuration
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // set counter
#ifdef PWM_ADC_TRIGER_SHIFT
    TIM_SetCounter(TIM1, PWM_PERIOD);
#endif

    // channel configuration
#ifdef PWM_AMP_TO_CHxN
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
#else
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
#endif
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
#ifdef PWM_CHx_ACTIVE_HIGH
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
#else
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
#endif
#ifdef PWM_CHxN_ACTIVE_HIGH
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
#else
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
#endif
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

    TIM_OCInitStructure.TIM_Pulse = PWM_PERIOD/2;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_Pulse = PWM_PERIOD/2;
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_Pulse = PWM_PERIOD/2;
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);

    // break module
    if (fault_handler!=0){
        TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
    } else {
        TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
    }
#ifdef PWM_BREAK_ACTIVE_HIGH
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
#else
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
#endif
    TIM_BDTRInitStructure.TIM_DeadTime = PWM_DEADTIME;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

    if (fault_handler!=0){
        // enable interrupt
        NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
    }

    // enable preloads
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_OC1PreloadConfig(TIM1, ENABLE);
    TIM_OC2PreloadConfig(TIM1, ENABLE);
    TIM_OC3PreloadConfig(TIM1, ENABLE);

    // configure interrupts
    TIM_ITConfig(TIM1, TIM_IT_Break, DISABLE);    // enable when PWM is enabled

    // store handler
    pwm.fault_handler = fault_handler;

    // output trigger
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);

    // TIM2 - ADC timing -------------------------------------------------------
    // Time base configuration
    TIM_TimeBaseStructure.TIM_Period = PWM_ADC_DIV+1;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_External1);

    // output trigger
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

    // enable timers
    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM2, ENABLE);

    // success
    return 0;

}

// -----------------------------------------------------------------------------
// enable
// -----------------------------------------------------------------------------
void pwm_enable(void){
    // enable break interrupt
    TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
    TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE);
    // enable pwms
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

// -----------------------------------------------------------------------------
// disable
// -----------------------------------------------------------------------------
void pwm_disable(void){
    // disable pwms
    TIM_CtrlPWMOutputs(TIM1, DISABLE);
    // disable break interrupt
    TIM_ITConfig(TIM1, TIM_IT_Break, DISABLE);
    TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void pwm_set(uint32_t * amp){
    if(amp[0]>F2Q(1,14)) return;
    if(amp[1]>F2Q(1,14)) return;
    if(amp[2]>F2Q(1,14)) return;
    TIM1->CCR1 = (amp[0]*PWM_PERIOD)>>14;            // CCR1 compare register
    TIM1->CCR2 = (amp[1]*PWM_PERIOD)>>14;            // CCR2 compare register
    TIM1->CCR3 = (amp[2]*PWM_PERIOD)>>14;            // CCR3 compare register
}

// -----------------------------------------------------------------------------
// break irq handler
// -----------------------------------------------------------------------------
void TIM1_BRK_TIM9_IRQHandler(void){
    // at this point, PWMs are automatically disabled by hardware
    // break is level based, not edge, so need to disable this interrupt to
    // avoid re-entry
    TIM_ITConfig(TIM1, TIM_IT_Break, DISABLE);
    TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
    // call handler
    if(pwm.fault_handler!= 0) pwm.fault_handler();
}

