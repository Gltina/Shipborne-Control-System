#ifndef _ENGINE775_H_
#define _ENGINE775_H_

#include "stm32f10x.h"

void Engine775_Init(void);

u8 Engine_control(u8 instruction);

void stop_normal();


#endif