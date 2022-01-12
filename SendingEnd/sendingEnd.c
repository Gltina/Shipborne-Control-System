#include "sendingEnd.h"

void SendingEnd_Init(void)
{
    TIM_Config();

    NVIC_Config();
}

void NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = SENDING_IRQ_TIMX;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM_Config(void)
{
    TIM_TimeBaseInitTypeDef TIMX_TimeBaseStructure;
    TIM_ICInitTypeDef TIMX_ICInitStructure;

    // 设置为200ms
    // 2000 次是一个周期
    // 记数频率为 72000000/7200 = 10000次
    // 验证: 2000*7200 / 36000000 = 0.2s
    RCC_APB1PeriphClockCmd(SENDING_RCC_TIMX, ENABLE); // 36MHZ

    TIM_DeInit(SENDING_TIMX);
    TIM_InternalClockConfig(TIM2);
    TIMX_TimeBaseStructure.TIM_Period = 1999;                    //0xffff;                  //设定计数器自动重装值（设置为最大）
    TIMX_TimeBaseStructure.TIM_Prescaler = 7199;                 //1;                    //设置分频系数
    TIMX_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割:TDTS = Tck_tim
    TIMX_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(SENDING_TIMX, &TIMX_TimeBaseStructure);

    TIM_ITConfig(SENDING_TIMX, TIM_IT_Update, ENABLE); //使能捕获和更新中断
    TIM_Cmd(SENDING_TIMX, ENABLE);                     //使能中断
}
