#ifndef _WATERTANK_H_
#define _WATERTANK_H_

#include "stm32f10x.h"

#define WATERTANK_CLOCK_RCC RCC_APB2Periph_GPIOB
#define WATERTANK_GPIOx GPIOB
#define WATERTANK_IN_Pin GPIO_Pin_3
#define WATERTANK_OUT_Pin GPIO_Pin_4

void WaterTank_Init();

// 进水口打开与关闭
void WaterTank_IN_Open();
void WaterTank_IN_Close();

// 出水口打开与关闭
// 电机与出口电磁阀一起作用
void WaterTank_OUT_Open();
void WaterTank_OUT_Close();

#endif