#include "engine775.h"

uint8_t curr_egine_status;

// 双775电机的频率可设计为  10khz  arr = 99  psc = 71
/**
	*  @brief This function initializes the pins and frequency of engine775
	*  @param s1_pin and s2_pin are the pins, arr and psc are the parameters to set the frequnecy of engine775
	*  @retval None
*/

void Engine775_Init(void)
{
    curr_egine_status = 's';
    
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIO外设时钟使能

	//设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    TIM_TimeBaseStructure.TIM_Period = 99;						//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
	TIM_TimeBaseStructure.TIM_Prescaler = 71;					//设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = DISABLE;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_ClearFlag(TIM1, TIM_FLAG_Update | TIM_FLAG_Trigger);
	TIM_ITConfig(TIM1, TIM_IT_Update | TIM_IT_Trigger, ENABLE);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			  //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;							  //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);					  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);					  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	TIM_CtrlPWMOutputs(TIM1, ENABLE); //MOE 主输出使能

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); //CH1预装载使能
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
	TIM_Cmd(TIM1, ENABLE);				//使能TIM1

	stop_normal();
}

void stop_normal(void)
{
	TIM_SetCompare1(TIM1, 0);
	TIM_SetCompare4(TIM1, 0);
}

void stop_emergency()
{
	TIM_SetCompare1(TIM1, 100);
	TIM_SetCompare4(TIM1, 100);
}

uint8_t judge_status(uint8_t new_status)
{
	//状态不变, 不需要操作
	if (curr_egine_status == new_status)
	{
		return 0;
	}
	//当前正转 而收到的指令为反转
	else if (curr_egine_status >= '0' && curr_egine_status <= '4' && new_status >= '5' && new_status <= '9')
	{
		return 1;
	}
	// 当前反转 而收到的指令为正转
	else if (curr_egine_status >= '5' && curr_egine_status <= '9' && new_status >= '0' && new_status <= '4')
	{
		return 2;
	}
	// 当前正转收到的指令也为正转（当前反转 收到的指令也为反转）
	// 进档
	else
	{
		return 3;
	}
}

void change_engine_status(uint8_t instruction)
{
	if (instruction == 's')
	{
		stop_normal();
	}
	else if (instruction == 'o')
	{
		stop_emergency();
	}

	// 正转的5个档位 '0'-'4'
	else if (instruction == '0')
	{

		TIM_SetCompare1(TIM1, 60);
		TIM_SetCompare4(TIM1, 0);
	}
	else if (instruction == '1')
	{
		TIM_SetCompare1(TIM1, 70);
		TIM_SetCompare4(TIM1, 0);
	}
	else if (instruction == '2')
	{
		TIM_SetCompare1(TIM1, 80);
		TIM_SetCompare4(TIM1, 0);
	}
	else if (instruction == '3')
	{
		TIM_SetCompare1(TIM1, 90);
		TIM_SetCompare4(TIM1, 0);
	}
	else if (instruction == '4')
	{
		TIM_SetCompare1(TIM1, 100);
		TIM_SetCompare4(TIM1, 0);
	}

	//反转的五个档位 '5'-'9'
	else if (instruction == '5')
	{
		TIM_SetCompare1(TIM1, 0);
		TIM_SetCompare4(TIM1, 60);
	}
	else if (instruction == '6')
	{
		TIM_SetCompare1(TIM1, 0);
		TIM_SetCompare4(TIM1, 70);
	}
	else if (instruction == '7')
	{

		TIM_SetCompare1(TIM1, 0);
		TIM_SetCompare4(TIM1, 80);
	}
	else if (instruction == '8')
	{

		TIM_SetCompare1(TIM1, 0);
		TIM_SetCompare4(TIM1, 90);
	}
	else if (instruction == '9')
	{
		TIM_SetCompare1(TIM1, 0);
		TIM_SetCompare4(TIM1, 100);
	}
	//无效的命令输出
	else
	{
		//printf("engine instruction invalid1\n");
	}
}

uint8_t Engine_Control(uint8_t instruction)
{
	uint8_t judge_result =
		judge_status(instruction);

	if (judge_result == 0)
	{
		// 重复指令
	}
	// 由正转反, 先停车在启动
	else if (judge_result == 1)
	{
		stop_normal();
		DELAY_ENGINE;
		change_engine_status(instruction);
	}
	// 由反转正, 先停车在启动
	else if (judge_result == 2)
	{
		stop_normal();
		DELAY_ENGINE;
		change_engine_status(instruction);
	}
    // 进档
	else if (judge_result == 3)
	{
		change_engine_status(instruction);
	}
    // 
	else
	{
		//printf("engine instruction invalid2\n");
	}

	// 更新当前状态
	curr_egine_status = instruction;

	return curr_egine_status;
}


uint8_t get_curr_engine_status()
{
    return curr_egine_status;
}
