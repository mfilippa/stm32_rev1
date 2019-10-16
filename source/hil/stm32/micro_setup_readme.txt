# STM32 setup

Define the folowing symbols in compiler:
    
    STM32F40_41xxx              // for STM32F415RG
    USE_STDPERIPH_DRIVER        // use peripheral driver

Change value of external crystal in stm32f4xx.h:

    #define HSE_VALUE    ((uint32_t)20000000) // Value of the External oscillator in Hz

Change value of PLL_M in system_stm32f4xx.c:

    #define PLL_M      20       // External oscillator / 1E6

