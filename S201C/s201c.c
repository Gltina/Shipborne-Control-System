#include "s201c.h"

void S201C_Init(void)
{
	TIM_Config();
    
	NVIC_Config();
    
	GPIO_Config();
}

void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = S201C_IRQ_TIMX;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef TIMX_TimeBaseStructure;
	TIM_ICInitTypeDef TIMX_ICInitStructure;
	
    // ����Ϊ200ms 
    // 2000 ����һ������ 
    // ����Ƶ��Ϊ 72000000/7200 = 10000��
    // ��֤: 2000*7200 / 36000000 = 0.2s
	RCC_APB1PeriphClockCmd(S201C_RCC_TIMX, ENABLE); // 36MHZ
    
    TIM_DeInit(S201C_TIMX);
    TIM_InternalClockConfig(TIM2);
    TIMX_TimeBaseStructure.TIM_Period = 1999;//0xffff;                  //�趨�������Զ���װֵ������Ϊ���
	TIMX_TimeBaseStructure.TIM_Prescaler = 7199;//1;                    //���÷�Ƶϵ��
	TIMX_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	        //����ʱ�ӷָ�:TDTS = Tck_tim
	TIMX_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(S201C_TIMX, &TIMX_TimeBaseStructure);			    

	TIMX_ICInitStructure.TIM_Channel = TIM_ChannelX;				 //ѡ�����벶�������ˣ�IC1ӳ�䵽TI1��
	TIMX_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	 //����Ϊ�����ز���
	TIMX_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIMX_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;			 //���������Ƶ������Ƶ
	TIMX_ICInitStructure.TIM_ICFilter = 0x00;						 //IC1F=0000 ���������˲������˴����˲�
	TIM_ICInit(S201C_TIMX, &TIMX_ICInitStructure);						 
    
    TIM_ClearITPendingBit(S201C_TIMX, TIM_IT_Update | S201C_IT_CCX); //���������жϱ�־λ
	TIM_ITConfig(S201C_TIMX, TIM_IT_Update | S201C_IT_CCX, ENABLE); //ʹ�ܲ���͸����ж�
	TIM_Cmd(S201C_TIMX, ENABLE);									//ʹ���ж�
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(S201C_RCC_GPIOX, ENABLE);

	GPIO_InitStructure.GPIO_Pin = S201C_GPIO_PinX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(S201C_GPIOX, &GPIO_InitStructure);

	GPIO_ResetBits(S201C_GPIOX, S201C_GPIO_PinX);
}
