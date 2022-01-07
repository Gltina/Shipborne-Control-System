#ifndef _S201C_H_
#define _S201C_H_

#include "stm32f10x.h"

#define S201C_TIMX TIM2
#define S201C_RCC_TIMX RCC_APB1Periph_TIM2
#define S201C_IRQ_TIMX TIM2_IRQn

#define S201C_GPIO_PinX GPIO_Pin_3
#define S201C_RCC_GPIOX RCC_APB2Periph_GPIOA
#define S201C_GPIOX GPIOA
#define S201C_IT_CCX TIM_IT_CC4
#define TIM_ChannelX TIM_Channel_4

void S201C_Init(void);

void NVIC_Config(void);
void TIM_Config(void);
void GPIO_Config(void);
#endif