#ifndef _SRH05_H
#define _SRH05_H

#include "stm32f10x.h"
#include "../delay/delay.h"

#define SRF05_CLOCK_RCC RCC_APB2Periph_GPIOA
#define GPIOx GPIOA
#define Trig_Pin GPIO_Pin_4
#define Echo_Pin GPIO_Pin_6

// Echo1 TIM3_Channel_1 PA6

void SRF05_Init(void);
//void SRF05_Trig(void);
void TIM3_SRF05_Init(u16 arr, u16 psc);
//uint16_t Get_Distance(float * distance_cm);

void right_TRIG_Start(void);

float Measure_distance(void);

#endif