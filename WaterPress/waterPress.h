#ifndef _WATERPRESS_H_
#define _WATERPRESS_H_

#include <stdlib.h>
#include <math.h>

#include "stm32f10x.h"

#include "../Algorithm/algorithm.h"
#include "../delay/delay.h"

#define ADC_TIMES 20

void WaterPress_Init(void);

uint16_t Get_WaterPress();

float Get_WaterPress_Filtered();

//void ADC_NVIC_Config(void);

float ADC2WaterDepth(uint16_t);

#endif