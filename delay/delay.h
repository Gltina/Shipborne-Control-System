#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f10x.h"

#define DELAY_MS delay_ms
#define DELAY_US delay_us

void delay_init(u8 SYSCLK);

void delay_ms(u16 nms);

void delay_us(u32 nus);

#endif