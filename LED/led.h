#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

// LED的电源开关, 若没打开此开关, 无法使用高电平触发
extern int LED_WORKING_ROOT;
extern int LED_HIGHBEAM_ROOT;
extern int LED_CAUTION_ROOT;

// 一共有三类灯
// LED1 为工作状态指示灯(一个蓝色灯)
// LED2 为前照灯(两个白色灯)
// LED3 为警示灯(两个红色灯)

// LED1 为工作状态指示灯(一个蓝色灯), 不可修改状态
#define LED1_GPIO_PORT GPIOB
#define LED1_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED1_GPIO_PIN GPIO_Pin_0

// LED2 为前照灯(两个白色灯)
#define LED2_GPIO_PORT GPIOB
#define LED2_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED2_GPIO_PIN GPIO_Pin_1

// LED3 为警示灯(两个红色灯), 不可修改状态
#define LED3_GPIO_PORT GPIOB
#define LED3_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED3_GPIO_PIN GPIO_Pin_5

/* 使用标准的固件库控制IO*/
#define LED_WORKING_OFF GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
#define LED_WORKING_ON         \
    if (LED_WORKING_ROOT == 1) \
    {GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);}

#define LED_HIGHBEAM_OFF GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);
#define LED_HIGHBEAM_ON         \
    if (LED_HIGHBEAM_ROOT == 1) \
    {GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);}

#define LED_CAUTION_OFF GPIO_ResetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
#define LED_CAUTION_ON         \
    if (LED_CAUTION_ROOT == 1) \
    {GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);}

void LED_Init(void);

void LED_period_control(uint8_t LED_index, uint32_t *period_count);

#endif /* __LED_H */
