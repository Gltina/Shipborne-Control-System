#ifndef _USART_H_
#define _USART_H_

#include <stdio.h>

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

#define USART RCC_APB2Periph_USART1
#define USART_TX RCC_APB2Periph_GPIOA

#define USART_AUX_GPIO_PIN GPIO_Pin_5
#define USART_TX_GPIO_PIN GPIO_Pin_9
#define USART_RX_GPIO_PIN GPIO_Pin_10
    
#define USART_AUX_GPIO_PORT GPIOA
#define USART_TX_GPIO_PORT GPIOA
#define USART_RX_GPIO_PORT GPIOA

#define USARTx USART1

void NVIC_Configuration(void);

void USART_Configuration(void);

uint32_t Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t c);
uint32_t Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t c);
uint32_t Usart_SendString(USART_TypeDef * pUSARTx, char *str); 
uint32_t Usart_SendByLength(USART_TypeDef *pUSARTx, char *str, uint32_t length);

int fputc(int ch, FILE *f);
int fgetc(FILE *f);

#endif
