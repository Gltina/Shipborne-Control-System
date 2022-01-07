#ifndef _MG90S_H_
#define _MG90S_H_

#include "stm32f10x.h"

void MG90S_Init(void);

void MG90S_TIM_Init(u16 arr, u16 psc);

void MG90S_GPIO_Init(void);

void Servo_Control(uint16_t angle);

#define CLK RCC_APB2Periph_GPIOB
#define PWM_GPIO_PIN GPIO_Pin_8
#define GPIO_PORT GPIOB

#endif