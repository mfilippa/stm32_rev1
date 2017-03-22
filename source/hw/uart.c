// -----------------------------------------------------------------------------
// uart.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "hal/uart.h"
#include "stm32f4xx.h"

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t uart_init(uart_t uart, uart_baud_t baud_rate){

    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uartdef;

    // check args
    if (baud_rate>=UART_BAUD_COUNT) return 1;
    if (uart>=UART_COUNT) return 1;

    // UART0
    if (uart == UART0) {
        // UART1: RX PB7, TX PB6

        // enable clocks
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

        // configure gpio
        gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
        gpio.GPIO_Mode = GPIO_Mode_AF;
        gpio.GPIO_OType = GPIO_OType_PP;
        gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
        gpio.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_Init(GPIOB, &gpio);

        // configure alternate function
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);

        // configure uart
        if (baud_rate==UART_BAUD_9600) uartdef.USART_BaudRate = 9600;
        if (baud_rate==UART_BAUD_115200) uartdef.USART_BaudRate = 115200;
        uartdef.USART_WordLength = USART_WordLength_8b;
        uartdef.USART_StopBits = USART_StopBits_1;
        uartdef.USART_Parity = USART_Parity_No;
        uartdef.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        uartdef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART1, &uartdef);

        // enable uart
        USART_Cmd(USART1, ENABLE);
    }

    // UART1
    if (uart == UART1) {
        // UART5: RX PD2, TX PC12

        // enable clocks
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

        // configure gpio
        gpio.GPIO_Pin = GPIO_Pin_2;
        gpio.GPIO_Mode = GPIO_Mode_AF;
        gpio.GPIO_OType = GPIO_OType_PP;
        gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
        gpio.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_Init(GPIOD, &gpio);
        gpio.GPIO_Pin = GPIO_Pin_12;
        GPIO_Init(GPIOC, &gpio);

        // configure alternate function
        GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);

        // configure uart
        if (baud_rate==UART_BAUD_9600) uartdef.USART_BaudRate = 9600;
        if (baud_rate==UART_BAUD_115200) uartdef.USART_BaudRate = 115200;
        uartdef.USART_WordLength = USART_WordLength_8b;
        uartdef.USART_StopBits = USART_StopBits_1;
        uartdef.USART_Parity = USART_Parity_No;
        uartdef.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        uartdef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(UART5, &uartdef);

        // enable uart
        USART_Cmd(UART5, ENABLE);
    }

    // success
    return 0;
}

// -----------------------------------------------------------------------------
// write ready
// -----------------------------------------------------------------------------
uint32_t uart_write_ready(uart_t uart){
    uint32_t ready = 0;
    if (uart==UART0) ready = ((USART1->SR & USART_FLAG_TXE) != 0);
    else if (uart==UART1) ready = ((UART5->SR & USART_FLAG_TXE) != 0);
    return ready;
}

// -----------------------------------------------------------------------------
// write
// -----------------------------------------------------------------------------
void uart_write(uart_t uart, uint32_t data){
    if (uart==UART0) USART1->DR = ((uint16_t)data & (uint16_t)0x01FF);
    else if (uart==UART1) UART5->DR = ((uint16_t)data & (uint16_t)0x01FF);
}

// -----------------------------------------------------------------------------
// read ready
// -----------------------------------------------------------------------------
uint32_t uart_read_ready(uart_t uart){
    uint32_t ready = 0;
    if (uart==UART0) ready = ((USART1->SR & USART_FLAG_RXNE) != 0);
    else if (uart==UART1) ready = ((UART5->SR & USART_FLAG_RXNE) != 0);
    return ready;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
uint32_t uart_read(uart_t uart){
    uint32_t data=0;
    if (uart==UART0) data = (uint32_t)(USART1->DR & (uint16_t)0x01FF);
    else if (uart==UART1) data = (uint32_t)(UART5->DR & (uint16_t)0x01FF);
    return data;
}
