#include "engine775.h"

uint8_t curr_egine_status;

// ˫775�����Ƶ�ʿ����Ϊ  10khz  arr = 99  psc = 71
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��GPIO����ʱ��ʹ��

	//���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    TIM_TimeBaseStructure.TIM_Period = 99;						//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler = 71;					//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = DISABLE;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ClearFlag(TIM1, TIM_FLAG_Update | TIM_FLAG_Trigger);
	TIM_ITConfig(TIM1, TIM_IT_Update | TIM_IT_Trigger, ENABLE);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			  //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;							  //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);					  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);					  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

	TIM_CtrlPWMOutputs(TIM1, ENABLE); //MOE �����ʹ��

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); //CH1Ԥװ��ʹ��
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM1, ENABLE);				//ʹ��TIM1

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
	//״̬����, ����Ҫ����
	if (curr_egine_status == new_status)
	{
		return 0;
	}
	//��ǰ��ת ���յ���ָ��Ϊ��ת
	else if (curr_egine_status >= '0' && curr_egine_status <= '4' && new_status >= '5' && new_status <= '9')
	{
		return 1;
	}
	// ��ǰ��ת ���յ���ָ��Ϊ��ת
	else if (curr_egine_status >= '5' && curr_egine_status <= '9' && new_status >= '0' && new_status <= '4')
	{
		return 2;
	}
	// ��ǰ��ת�յ���ָ��ҲΪ��ת����ǰ��ת �յ���ָ��ҲΪ��ת��
	// ����
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

	// ��ת��5����λ '0'-'4'
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

	//��ת�������λ '5'-'9'
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
	//��Ч���������
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
		// �ظ�ָ��
	}
	// ����ת��, ��ͣ��������
	else if (judge_result == 1)
	{
		stop_normal();
		DELAY_ENGINE;
		change_engine_status(instruction);
	}
	// �ɷ�ת��, ��ͣ��������
	else if (judge_result == 2)
	{
		stop_normal();
		DELAY_ENGINE;
		change_engine_status(instruction);
	}
    // ����
	else if (judge_result == 3)
	{
		change_engine_status(instruction);
	}
    // 
	else
	{
		//printf("engine instruction invalid2\n");
	}

	// ���µ�ǰ״̬
	curr_egine_status = instruction;

	return curr_egine_status;
}


uint8_t get_curr_engine_status()
{
    return curr_egine_status;
}
