// -----------------------------------------------------------------------------
// dac.c - MPF 10/2021
// -----------------------------------------------------------------------------

// includes
#include "stm32f4xx.h"
#include "dac.h"

// -----------------------------------------------------------------------------
// dac configuration
// 
// 2 DAC channels, 12bit triggered by Timer 2 TRGO, same trigger used for ADC 
// and configured in pwm.c 
//
// GPIO used - shared with ADC
// PA4 - DAC_CH1 - ADC_IN4
// PA5 - DAC_CH2 - ADC_IN5

// comment to disable
#define DAC_CH1_EN
#define DAC_CH2_EN

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
void dac_init(dac_trigger_t trigger){

    GPIO_InitTypeDef GPIO_InitStruct;
    DAC_InitTypeDef DAC_InitStruct;

    // enable all required peripheral clocks
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // configure gpio
    GPIO_InitStruct.GPIO_Pin = 0;
    #ifdef DAC_CH1_EN
    GPIO_InitStruct.GPIO_Pin |= GPIO_Pin_4;
    #endif
    #ifdef DAC_CH2_EN
    GPIO_InitStruct.GPIO_Pin |= GPIO_Pin_5;
    #endif
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // DAC channel configuration
    if(trigger==DAC_TRIGGER_PWM) DAC_InitStruct.DAC_Trigger = DAC_Trigger_T2_TRGO;
    else if (trigger==DAC_TRIGGER_SW) DAC_InitStruct.DAC_Trigger = DAC_Trigger_Software;
    else DAC_InitStruct.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    #ifdef DAC_CH1_EN
    DAC_Init(DAC_Channel_1, &DAC_InitStruct);
    #endif
    #ifdef DAC_CH2_EN
    DAC_Init(DAC_Channel_2, &DAC_InitStruct);
    #endif

    // enable DAC
    #ifdef DAC_CH1_EN
    DAC_Cmd(DAC_Channel_1, ENABLE);
    #endif
    #ifdef DAC_CH2_EN
    DAC_Cmd(DAC_Channel_2, ENABLE);
    #endif
}

// -----------------------------------------------------------------------------
// set
// -----------------------------------------------------------------------------
void dac_set(dac_ch_t channel, int32_t value){
    // no error checking for speed
    // expected value range 0..2^16-1
    if (channel==DAC_CH1) 
        DAC_SetChannel1Data(DAC_Align_12b_R, 0x0FFF&(value>>4));
    if (channel==DAC_CH2) 
        DAC_SetChannel2Data(DAC_Align_12b_R, 0x0FFF&(value>>4));
}

// -----------------------------------------------------------------------------
// software trigger
// -----------------------------------------------------------------------------
void dac_sw_trigger(void){
    // trigger both channels
    DAC_DualSoftwareTriggerCmd(ENABLE);
}