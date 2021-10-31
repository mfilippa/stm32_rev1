// -----------------------------------------------------------------------------
// pwm.h - MPF 10/2021
// -----------------------------------------------------------------------------
// 3 phase PWM module with BRK input
// -----------------------------------------------------------------------------

// includes
#include "stm32f4xx.h"
#include "pwm.h"
#include "macros.h"

// -----------------------------------------------------------------------------
// pwm configuration
// -----------------------------------------------------------------------------
// GPIO comment unused pins
#define PA8      // TMR1_CH1     HIN_A
#define PB13     // TMR1_CH1N   /LIN_A
#define PA9      // TMR1_CH2     HIN_B
#define PB14     // TMR1_CH2N   /LIN_B
#define PA10     // TMR1_CH3     HIN_C
#define PB15     // TMR1_CH3N   /LIN_C
#define PB12     // TMR1_BKIN    SD/OC

// comment the following line to map duty to CHx
#define PWM_AMP_TO_CHxN

// comment the following line for active low
#define PWM_CHx_ACTIVE_HIGH
#define PWM_CHxN_ACTIVE_HIGH
#define PWM_BREAK_ACTIVE_HIGH

// comment this line to adjust ADC trigger alignment to PWM
//#define PWM_ADC_TRIGER_SHIFT

// TMR1: PWMs
// TMR2: slow ADC timing
// fast ADC trigger runs at PWM freq

// slow ADC trigger frequency
#define PWM_ADC_DIV        2        // ADC_FREQ = PWM_FREQUENCY/(2+PWM_ADC_DIV)

// -----------------------------------------------------------------------------

// module structure
struct pwm_struct {
    void (*fault_handler)(void);
    uint32_t period;
} pwm;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void pwm_init(uint32_t freq, uint32_t deadtime, void (*fault_handler)(void)){

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

    // calculate and store period - will need when changing freq
    pwm.period = SystemCoreClock/freq/2;

    // enable clocks to peripherals
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // GPIO Configuration
    GPIO_InitStructure.GPIO_Pin = 0;
    #ifdef PA8
    GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_8;
    #endif
    #ifdef PA9
    GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_9;
    #endif
    #ifdef PA10
    GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_10;
    #endif
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = 0;
    #ifdef PB14
    GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_13;
    #endif
    #ifdef PB14
    GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_14;
    #endif
    #ifdef PB15
    GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_15;
    #endif
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #ifdef PB12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    #endif
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // break input

    // AF configuration
    #ifdef PA8
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8, GPIO_AF_TIM1);
    #endif
    #ifdef PA9
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_TIM1);
    #endif
    #ifdef PA10
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10, GPIO_AF_TIM1);
    #endif
    #ifdef PB13
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13, GPIO_AF_TIM1);
    #endif
    #ifdef PB14
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14, GPIO_AF_TIM1);
    #endif
    #ifdef PB15
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15, GPIO_AF_TIM1);
    #endif
    #ifdef PB12
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource12, GPIO_AF_TIM1);      // break input
    #endif
    
    // Time base configuration
    TIM_TimeBaseStructure.TIM_Period = pwm.period;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // set counter
#ifdef PWM_ADC_TRIGER_SHIFT
    TIM_SetCounter(TIM1, pwm.period);
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

    TIM_OCInitStructure.TIM_Pulse = pwm.period/2;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_Pulse = pwm.period/2;
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_Pulse = pwm.period/2;
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
    TIM_BDTRInitStructure.TIM_DeadTime = deadtime;
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
// set duty q14
// -----------------------------------------------------------------------------
void pwm_set_duty_q(uint32_t * duty_q14){
    // no range checking for speed
    TIM1->CCR1 = (duty_q14[0]*pwm.period)>>14;          // CCR1 compare register
    TIM1->CCR2 = (duty_q14[1]*pwm.period)>>14;          // CCR2 compare register
    TIM1->CCR3 = (duty_q14[2]*pwm.period)>>14;          // CCR3 compare register
}

// -----------------------------------------------------------------------------
// set duty float
// -----------------------------------------------------------------------------
void pwm_set_duty_f(float * duty){
    // no range checking for speed
    TIM1->CCR1 = (uint32_t)(duty[0]*(float)pwm.period); // CCR1 compare register
    TIM1->CCR2 = (uint32_t)(duty[1]*(float)pwm.period); // CCR2 compare register
    TIM1->CCR3 = (uint32_t)(duty[2]*(float)pwm.period); // CCR3 compare register
}


// -----------------------------------------------------------------------------
// set frequency
// -----------------------------------------------------------------------------
void pwm_set_freq(uint32_t freq){
    // no range checking for speed
    pwm.period = SystemCoreClock/freq/2;
    TIM1->ARR = pwm.period;         // auto reload register
}

// -----------------------------------------------------------------------------
// set deadtime
// -----------------------------------------------------------------------------
void pwm_set_deadtime(uint32_t deadtime){
    // no range checking for speed
    TIM1->BDTR = (TIM1->BDTR&0xFF00)|(deadtime&0x00FF);
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

