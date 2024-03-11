// app

    // disable all interrupts
    sys_disable_interrupts();

    // init systick
    sys_tick_init(SYSTICK_FREQ_HZ, &app_systick);

    // init peripherals
    io_init();
    uart_init(UART_COMM,UART_BAUD_115200);

    // init pwm
    pwm_init(10000,0, 0);  // freq, deadtime, fault handler
    pwm_enable();

    // init adc
    adc_init(&app_adc_fast_handler,
        &app_adc_slow_handler);     // fast handler, slow handler

    // init dac
    dac_init(DAC_TRIGGER_SW);

    // enable interrupts
    sys_enable_interrupts();


// modules that need work
debug
fb
fsm
macros
mathlib
param
sm