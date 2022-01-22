#include "rudderControl.h"

void MG90S_Init()
{
    MG90S_GPIO_Init();

    // ����Ϊ20ms, (arr+1)(psc+1)/Tick = 20000 * 72 / 72000000=0.02s(20ms)
    // ����Ϊ2.5ms, (arr+1)(psc+1)/Tick = 2500 * 72 / 72000000=0.0025s(2.5ms)
    MG90S_TIM_Init(19999, 71);
    
}

void MG90S_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(Rudder_CLK, ENABLE); //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
    GPIO_InitStructure.GPIO_Pin = Rudder0_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Rudder0_GPIO_PORT, &GPIO_InitStructure);
}

void MG90S_TIM_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        //ʹ�ܶ�ʱ��3ʱ��
    
    TIM_TimeBaseStructure.TIM_Period = arr;                     //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָ�:TDTS = Tck_tim
                                                                //TIM_TimeBaseStructure.TIM_RepetitionCounter = DISABLE;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //TIM_ClearFlag(TIM3, TIM_FLAG_Update | TIM_FLAG_Trigger);
    //TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_Trigger, ENABLE);

    //��ʼ��TIM3 Channel2 PWMģʽ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //�������:TIM����Ƚϼ��Ը�

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);                    //����Tָ���Ĳ�����ʼ������TIM3 OC2
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);                    //����Tָ���Ĳ�����ʼ������TIM3 OC2
    
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE); //ʹ��TIM3
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

