#include "mg90s.h"

void MG90S_Init()
{
    MG90S_GPIO_Init();

    // ����Ϊ20ms, (arr+1)(psc+1)/Tick = 20000 * 72 / 72000000=0.02s(20ms)

    // ����Ϊ2.5ms, (arr+1)(psc+1)/Tick = 2500 * 72 / 72000000=0.0025s(2.5ms)
    MG90S_TIM_Init(19999, 71);
}

void MG90S_GPIO_Init(void)
{
    //GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PC7
    //GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

    //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5
}

void MG90S_TIM_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʹ�ܶ�ʱ��4ʱ��

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
    //GPIO_PinRemapConfig(GPIO_FullRemap_TIM5, ENABLE);

    GPIO_InitStructure.GPIO_Pin = PWM_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_PORT, &GPIO_InitStructure); //��ʼ��GPIO

    //GPIO_ResetBits(GPIO_PORT,PWM_GPIO_PIN);
    //GPIO_SetBits(GPIO_PORT, PWM_GPIO_PIN);

    //��ʼ��TIM3
    TIM_TimeBaseStructure.TIM_Period = arr;                     //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָ�:TDTS = Tck_tim
                                                                //TIM_TimeBaseStructure.TIM_RepetitionCounter = DISABLE;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);             //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ClearFlag(TIM4, TIM_FLAG_Update | TIM_FLAG_Trigger);
    TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_Trigger, ENABLE);

    //��ʼ��TIM3 Channel2 PWMģʽ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //�������:TIM����Ƚϼ��Ը�

    TIM_OC3Init(TIM4, &TIM_OCInitStructure); //����Tָ���Ĳ�����ʼ������TIM3 OC2
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE); //ʹ��TIM3
}

void Servo_Control(uint16_t angle)
{
    // 0.5ms -> 0��
    // 1.0ms -> 45��
    // 1.5ms -> 90��
    // 2.0ms -> 135��
    // 2.5ms -> 180��

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
