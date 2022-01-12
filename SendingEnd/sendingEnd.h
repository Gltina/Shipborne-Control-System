#ifndef _SENDINGEND_H_
#define _SENDINGEND_H_

#include "stm32f10x.h"

#define SENDING_TIMX TIM2
#define SENDING_RCC_TIMX RCC_APB1Periph_TIM2
#define SENDING_IRQ_TIMX TIM2_IRQn

void SendingEnd_Init(void);

void NVIC_Config(void);
void TIM_Config(void);

#endif