// -----------------------------------------------------------------------------
// uart.c - MPF 11/2019
// -----------------------------------------------------------------------------

// includes
#include "error.h"
#include "uart.h"
#include "stm32f10x.h"

// uart config table
typedef struct uart_config_struct {
    USART_TypeDef * uart;       // uart: USART1,2,3
    GPIO_TypeDef * rx_port;     // port: GPIOA..D
    uint16_t rx_pin;            // pin: GPIO_Pin_0..15
    GPIO_TypeDef * tx_port;     // port: GPIOA..D
    uint16_t tx_pin;            // pin: GPIO_Pin_0..15
} uart_config_t;

// -----------------------------------------------------------------------------
// uart configuration
uart_config_t uart_cfg[UART_COUNT] = {
//  uart,   rx_port, rx_pin,     tx_port, tx_pin,     
    {USART1, GPIOB,   GPIO_Pin_7, GPIOB,   GPIO_Pin_6}, // UART_COMM
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
void uart_init(uart_t uart, uart_baud_t baud_rate){

    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uartdef;

    // check args and pins - ports and uarts are checked at init functions
    if (baud_rate>=UART_BAUD_COUNT) error_raise(ERROR_UART_INIT);
    if (uart>=UART_COUNT) error_raise(ERROR_UART_INIT);
    if (!IS_GPIO_PIN(uart_cfg[uart].rx_pin)) error_raise(ERROR_UART_INIT);
    if (!IS_GPIO_PIN(uart_cfg[uart].tx_pin)) error_raise(ERROR_UART_INIT);

    // enable all peripherals
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    // enable uart clocks - also range check UART
    if (uart_cfg[uart].uart==USART1) 
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    else if (uart_cfg[uart].uart==USART2) 
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    else if (uart_cfg[uart].uart==USART3) 
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // configure gpio
    gpio.GPIO_Pin = uart_cfg[uart].rx_pin;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(uart_cfg[uart].rx_port, &gpio);
    gpio.GPIO_Pin = uart_cfg[uart].tx_pin;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(uart_cfg[uart].tx_port, &gpio);

    // configure alternate function
    if (uart_cfg[uart].uart==USART1) 
        GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
    if (uart_cfg[uart].uart==USART2) 
        GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    if (uart_cfg[uart].uart==USART3) 
        GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);

    // configure uart
    if (baud_rate==UART_BAUD_9600) uartdef.USART_BaudRate = 9600;
    if (baud_rate==UART_BAUD_115200) uartdef.USART_BaudRate = 115200;
    uartdef.USART_WordLength = USART_WordLength_8b;
    uartdef.USART_StopBits = USART_StopBits_1;
    uartdef.USART_Parity = USART_Parity_No;
    uartdef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    uartdef.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(uart_cfg[uart].uart, &uartdef);

    // enable uart
    USART_Cmd(uart_cfg[uart].uart, ENABLE);

}

// -----------------------------------------------------------------------------
// write ready
// -----------------------------------------------------------------------------
bool uart_write_ready(uart_t uart){
    return (uart_cfg[uart].uart->SR & USART_FLAG_TXE) != 0;
}

// -----------------------------------------------------------------------------
// write
// -----------------------------------------------------------------------------
void uart_write(uart_t uart, uint32_t data){
    uart_cfg[uart].uart->DR = ((uint16_t)data & (uint16_t)0x01FF);
}

// -----------------------------------------------------------------------------
// read ready
// -----------------------------------------------------------------------------
bool uart_read_ready(uart_t uart){
    return (uart_cfg[uart].uart->SR & USART_FLAG_RXNE) != 0;
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
uint32_t uart_read(uart_t uart){
    return (uint32_t)(uart_cfg[uart].uart->DR & (uint16_t)0x01FF);
}
