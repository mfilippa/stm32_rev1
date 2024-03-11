// -----------------------------------------------------------------------------
// error.h - Mariano F - 04/2024
// -----------------------------------------------------------------------------

#ifndef _ERROR_H_
#define _ERROR_H_

// public includes
#include <stdint.h>
#include <bool.h>
#include <string.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// error definitions
typedef enum error_enum {
    ERROR_NONE=0,
    ERROR_MAIN,         // main.c
    ERROR_APP,          // app module
    ERROR_IO,           // io module
    ERROR_PWM,          // pwm module
    ERROR_UART,         // uart module
    ERROR_ADC,          // adc module
    ERROR_SYS,          // sys module
    ERROR_I2C,          // i2c module
    ERROR_COMM,         // comm module
    ERROR_SCH,          // sch module
} error_t;

// error handler prototype
typedef void (error_handler_t)(error_t err);

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init - pass a function to handle errors, NULL to disable
void error_init(error_handler_t * handler);

// raise error
void error_raise(error_t err);

#endif // _ERROR_H_
