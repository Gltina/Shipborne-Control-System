#include "mg90s.h"

void MG90S_Init()
{
    MG90S_GPIO_Init();

    // 周期为20ms, (arr+1)(psc+1)/Tick = 20000 * 72 / 72000000=0.02s(20ms)

    // 周期为2.5ms, (arr+1)(psc+1)/Tick = 2500 * 72 / 72000000=0.0025s(2.5ms)
    MG90S_TIM_Init(19999, 71);
}

void MG90S_GPIO_Init(void)
{
    //GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PC7
    //GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

    //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
}

void MG90S_TIM_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //使能定时器4时钟

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能GPIO外设和AFIO复用功能模块时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
    //GPIO_PinRemapConfig(GPIO_FullRemap_TIM5, ENABLE);

    GPIO_InitStructure.GPIO_Pin = PWM_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_PORT, &GPIO_InitStructure); //初始化GPIO

    //GPIO_ResetBits(GPIO_PORT,PWM_GPIO_PIN);
    //GPIO_SetBits(GPIO_PORT, PWM_GPIO_PIN);

    //初始化TIM3
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割:TDTS = Tck_tim
                                                                //TIM_TimeBaseStructure.TIM_RepetitionCounter = DISABLE;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);             //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_ClearFlag(TIM4, TIM_FLAG_Update | TIM_FLAG_Trigger);
    TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_Trigger, ENABLE);

    //初始化TIM3 Channel2 PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //输出极性:TIM输出比较极性高

    TIM_OC3Init(TIM4, &TIM_OCInitStructure); //根据T指定的参数初始化外设TIM3 OC2
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE); //使能TIM3
}

void Servo_Control(uint16_t angle)
{
    // 0.5ms -> 0度
    // 1.0ms -> 45度
    // 1.5ms -> 90度
    // 2.0ms -> 135度
    // 2.5ms -> 180度

    if (angle == 0)
        TIM_SetCompare3(TIM4, 500);
    else if (angle == 90)
        TIM_SetCompare3(TIM4, 1500);
    else if (angle == 180)
        TIM_SetCompare3(TIM4, 2500);
    else
    {
        printf("<MG90S error> angle error\r\n");
    }
}

void set_angle(uint8_t signal)
{
    if (signal == 0)
    {
        Servo_Control(0);
    }
    else if (signal == 1)
    {
        Servo_Control(90);
    }
    else if (signal == 2)
    {
        Servo_Control(180);
    }
}
