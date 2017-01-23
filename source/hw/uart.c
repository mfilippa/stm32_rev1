// -----------------------------------------------------------------------------
// uart.c - MPF 01/2017
// -----------------------------------------------------------------------------

// includes
#include "hal/uart.h"
#include "stm32f4xx.h"

// hardware description for STM32F4
// APB2 - FCLK2 84MHz
// PB6 - UART1_TX - AF7
// PB7 - UART1_RX - AF7

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
uint32_t uart_init(uint32_t baud_rate){

	GPIO_InitTypeDef gpio;
	USART_InitTypeDef uart;

	// check baudrate
	if ((baud_rate!=9600)&&(baud_rate!=115200)) return 1;

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
	uart.USART_BaudRate = baud_rate;
	uart.USART_WordLength = USART_WordLength_8b;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &uart);

	// enable uart
	USART_Cmd(USART1, ENABLE);

	// success
	return 0;
}

// -----------------------------------------------------------------------------
// write ready
// -----------------------------------------------------------------------------
uint32_t uart_write_ready(void){
	return ((USART1->SR & USART_FLAG_TXE) != 0);
}

// -----------------------------------------------------------------------------
// write
// -----------------------------------------------------------------------------
void uart_write(uint32_t data){
	USART1->DR = ((uint16_t)data & (uint16_t)0x01FF);
}

// -----------------------------------------------------------------------------
// read ready
// -----------------------------------------------------------------------------
uint32_t uart_read_ready(void){
	return ((USART1->SR & USART_FLAG_RXNE) != 0);
}

// -----------------------------------------------------------------------------
// read
// -----------------------------------------------------------------------------
uint32_t uart_read(void){
	return (uint32_t)(USART1->DR & (uint16_t)0x01FF);
}
