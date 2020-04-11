// -----------------------------------------------------------------------------
// error.h - MPF 11/2019
// -----------------------------------------------------------------------------

#ifndef _ERROR_H_
#define _ERROR_H_

// error definitions
typedef enum error_enum {
    ERROR_NONE=0,
    ERROR_INIT,         // generic 
    ERROR_ARG,          // 
    ERROR_SYS_INIT,     // sys module
    ERROR_SYS_ARG,
    ERROR_IO_INIT,      // io module
    ERROR_IO_ARG,
    ERROR_UART_INIT,    // uart module
    ERROR_UART_ARG,
    ERROR_PWM_INIT,     // pwm module
    ERROR_PWM_ARG,
    ERROR_ADC_INIT,     // adc module
    ERROR_ADC_ARG,
    ERROR_I2C_INIT,     // i2c module
    ERROR_I2C_ARG,
    ERROR_APP_INIT,     // app module
    ERROR_APP_ARG,
    ERROR_COMM_INIT,    // comm module
    ERROR_COMM_ARG,
    ERROR_SCH_INIT,     // sch module
    ERROR_SCH_ARG,
    ERROR_SCH_REG, 
} error_t;

// init
// pass a function to handle errors, NULL to disable
// init should happen in main.c and handler should live in main.c to have 
// multiple implementations for every platform. E.g. for simulation, it can 
// raise an error flag. For a micro, it can force it to get stuck in a loop
void error_init(void (*err_handler)(error_t err));

// raise error
void error_raise(error_t err);

#endif // _ERROR_H_
