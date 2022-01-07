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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级
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
	RCC_APB1PeriphClockCmd(S201C_RCC_TIMX, ENABLE); // 36MHZ
    
    TIM_DeInit(S201C_TIMX);
    TIM_InternalClockConfig(TIM2);
    TIMX_TimeBaseStructure.TIM_Period = 1999;//0xffff;                  //设定计数器自动重装值（设置为最大）
	TIMX_TimeBaseStructure.TIM_Prescaler = 7199;//1;                    //设置分频系数
	TIMX_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	        //设置时钟分割:TDTS = Tck_tim
	TIMX_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //TIM向上计数模式
	TIM_TimeBaseInit(S201C_TIMX, &TIMX_TimeBaseStructure);			    

	TIMX_ICInitStructure.TIM_Channel = TIM_ChannelX;				 //选择输入捕获的输入端，IC1映射到TI1上
	TIMX_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	 //设置为上升沿捕获
	TIMX_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIMX_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;			 //配置输入分频，不分频
	TIMX_ICInitStructure.TIM_ICFilter = 0x00;						 //IC1F=0000 配置输入滤波器，此处不滤波
	TIM_ICInit(S201C_TIMX, &TIMX_ICInitStructure);						 
    
    TIM_ClearITPendingBit(S201C_TIMX, TIM_IT_Update | S201C_IT_CCX); //清除捕获和中断标志位
	TIM_ITConfig(S201C_TIMX, TIM_IT_Update | S201C_IT_CCX, ENABLE); //使能捕获和更新中断
	TIM_Cmd(S201C_TIMX, ENABLE);									//使能中断
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
