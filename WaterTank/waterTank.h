#ifndef _WATERTANK_H_
#define _WATERTANK_H_

#include "stm32f10x.h" 

#define WATERTANK_DOOR_CLOCK_RCC RCC_APB2Periph_GPIOB
#define WATERTANK_DOOR_GPIOx GPIOB
#define WATERTANK_DOOR_IN_Pin GPIO_Pin_14
#define WATERTANK_DOOR_OUT_Pin GPIO_Pin_15

#define WATERTANK_LEVEL_CLOCK_RCC RCC_APB2Periph_GPIOA
#define WATERTANK_LEVEL_GPIOx GPIOA
#define WATERTANK_LEVEL0_Pin GPIO_Pin_3
#define WATERTANK_LEVEL1_Pin GPIO_Pin_4

void WaterTank_Init();

// 进水口打开与关闭
void WaterTank_IN_Open();
void WaterTank_IN_Close();

// 出水口打开与关闭
// 电机与出口电磁阀一起作用
void WaterTank_OUT_Open();
void WaterTank_OUT_Close();

// 获取水位值
void Get_WaterTank_SensorData(float * s1, float *s2);
void Get_WaterLevel_WaterTank(float * water_level_0, float *water_level_1);

void set_waterTank_IN_status(uint8_t status);
void set_waterTank_OUT_status(uint8_t status);

#endif