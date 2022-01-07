#include "waterTank.h"

void WaterTank_Init()
{
    RCC_APB2PeriphClockCmd(WATERTANK_CLOCK_RCC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = WATERTANK_IN_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(WATERTANK_GPIOx, WATERTANK_IN_Pin);

    GPIO_InitStructure.GPIO_Pin = WATERTANK_OUT_Pin;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(WATERTANK_GPIOx, WATERTANK_OUT_Pin);
}

void WaterTank_IN_Open()
{
    GPIO_SetBits(WATERTANK_GPIOx, WATERTANK_IN_Pin);
}

void WaterTank_IN_Close()
{
    GPIO_ResetBits(WATERTANK_GPIOx, WATERTANK_IN_Pin);
}

void WaterTank_OUT_Open()
{
    GPIO_SetBits(WATERTANK_GPIOx, WATERTANK_OUT_Pin);
}
void WaterTank_OUT_Close()
{
    GPIO_ResetBits(WATERTANK_GPIOx, WATERTANK_OUT_Pin);
}