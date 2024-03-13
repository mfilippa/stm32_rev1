// -----------------------------------------------------------------------------
// adc.h - Mariano F - 04/2024
// -----------------------------------------------------------------------------

#ifndef _ADC_H_
#define _ADC_H_

// public includes
#include <stdint.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// adc channels
typedef enum adc_ch_enum {
    ADC_CH1,
    ADC_CH2,
    ADC_CH3,
    ADC_CH_COUNT,
} adc_ch_t;

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init
void adc_init(void);

// force a trigger
void adc_trigger(void);

// read an adc channel
uint32_t adc_read(adc_ch_t channel);

// comm handler
void adc_comm_handler(uint32_t rx_size);

#endif // _ADC_H_
