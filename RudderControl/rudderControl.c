#include "rudderControl.h"

void MG90S_Init()
{
    MG90S_GPIO_Init();

    // 周期为20ms, (arr+1)(psc+1)/Tick = 20000 * 72 / 72000000=0.02s(20ms)
    // 周期为2.5ms, (arr+1)(psc+1)/Tick = 2500 * 72 / 72000000=0.0025s(2.5ms)
    MG90S_TIM_Init(19999, 71);
    
}

void MG90S_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(Rudder_CLK, ENABLE); //使能GPIO外设和AFIO复用功能模块时钟
    GPIO_InitStructure.GPIO_Pin = Rudder0_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Rudder0_GPIO_PORT, &GPIO_InitStructure);
}

void MG90S_TIM_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        //使能定时器3时钟
    
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割:TDTS = Tck_tim
                                                                //TIM_TimeBaseStructure.TIM_RepetitionCounter = DISABLE;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //TIM_ClearFlag(TIM3, TIM_FLAG_Update | TIM_FLAG_Trigger);
    //TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_Trigger, ENABLE);

    //初始化TIM3 Channel2 PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //输出极性:TIM输出比较极性高

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);                    //根据T指定的参数初始化外设TIM3 OC2
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);                    //根据T指定的参数初始化外设TIM3 OC2
    
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE); //使能TIM3
}

void Rudder0_set_angle(uint8_t signal)
{
    if (signal == 0)
    {
        TIM_SetCompare1(TIM3, 500);
    }
    else if (signal == 1)
    {
        TIM_SetCompare1(TIM3, 1500);
    }
    else if (signal == 2)
    {
        TIM_SetCompare1(TIM3, 2500);
    }
}

void Rudder1_set_angle(uint8_t signal)
{
    if (signal == 0)
    {
        TIM_SetCompare2(TIM3, 500);
    }
    else if (signal == 1)
    {
        TIM_SetCompare2(TIM3, 1500);
    }
    else if (signal == 2)
    {
        TIM_SetCompare2(TIM3, 2500);
    }
}

