#ifndef _MG90S_H_
#define _MG90S_H_

#include "stm32f10x.h"

#define Rudder_CLK RCC_APB2Periph_GPIOA
#define Rudder0_GPIO_PORT GPIOA
#define Rudder1_GPIO_PORT GPIOA
#define Rudder0_GPIO_PIN GPIO_Pin_6
#define Rudder1_GPIO_PIN GPIO_Pin_7

void MG90S_Init(void);

void MG90S_TIM_Init(u16 arr, u16 psc);

void MG90S_GPIO_Init(void);

// 0.5ms -> 0業
// 1.0ms -> 45業
// 1.5ms -> 90業
// 2.0ms -> 135業
// 2.5ms -> 180業
void Rudder0_set_angle(uint8_t signal);

void Rudder1_set_angle(uint8_t signal);

void Servo_Control(uint16_t angle);


#endif