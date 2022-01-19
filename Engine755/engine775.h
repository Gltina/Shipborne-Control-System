#ifndef _ENGINE775_H_
#define _ENGINE775_H_

#include "stm32f10x.h"
#include "../delay/delay.h"

/*

    static uint8_t curr_egine_status = 's'; // 默认为停电关机状态

    电机控制分为以下几个档位:
    's': 停电关机
    'o': 上电急停
    '0' - '4': 正转档位
    '5' - '9': 反转档位
*/

#define DELAY_ENGINE DELAY_MS(500)

void Engine775_Init(void);

// 接受来自上位机(PC)的信号, 对比当前状态调整电机动作
uint8_t Engine_Control(uint8_t instruction);

/* 
    对比当前状态与新指令, 确定电机动作
    @return:
        0 状态不变, 不需要操作
        1 当前正转 而收到的指令为反转
        2 当前反转 而收到的指令为正转
        3 同方向进档
*/
uint8_t judge_status(uint8_t new_status);

void change_engine_status(uint8_t instruction);

// 停电关机
void stop_normal();

// 上电关机, 急停
void stop_emergency();

#endif