#ifndef _WATERPRESS_H_
#define _WATERPRESS_H_ 

#include <stdlib.h>
#include <math.h>

#include "stm32f10x.h"

#include "../Algorithm/algorithm.h"
#include "../delay/delay.h"

#define WATERPRESS_CLOCK_RCC RCC_APB2Periph_GPIOA
#define WATERPRESS_GPIOx GPIOA
#define WATERPRESS_Pin0 GPIO_Pin_1
#define WATERPRESS_Pin1 GPIO_Pin_2

void WaterPress_Init(void);

//uint16_t Get_WaterPress();

//float Get_WaterPress_Filtered();

//void ADC_NVIC_Config(void);

//float ADC2WaterDepth(uint16_t);

void Get_WaterPress_SensorData(float * s1, float * s2);

void Get_WaterDepth(float * water_depth0, float * water_depth1);


#endif