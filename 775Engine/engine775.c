#include "../DeviceManage/deviceManage.h"
#include "engine775.h"


// ˫775�����Ƶ�ʿ����Ϊ  10khz  arr = 99  psc = 71
/**
	*  @brief This function initializes the pins and frequency of engine775
	*  @param s1_pin and s2_pin are the pins, arr and psc are the parameters to set the frequnecy of engine775
	*	 @retval None
*/
void ENGINE775_Init(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //ʹ��GPIO����ʱ��ʹ��
	                                                                     	

   //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11; 			//TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  	//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period = 99; 			//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =71; 			//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 		//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = DISABLE;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 	//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update | TIM_FLAG_Trigger);
  TIM_ITConfig(TIM1, TIM_IT_Update|TIM_IT_Trigger, ENABLE);
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 	//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; 			//���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��	
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��	 
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable); 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
	
	TIM_SetCompare1(TIM1,0);
	TIM_SetCompare4(TIM1,0);
}

u8 Engine_control(u8 instruction)
{
		// ֹͣ
		if (instruction == 's')
		{
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,0);
				return('S');
		}
		//ɲ��
		else if(instruction == 'o')
		{
				TIM_SetCompare1(TIM1,100);
				TIM_SetCompare4(TIM1,100);
				// ��д��O
				return('O');
		}
		// ��ת��5����λ 0-4
		else if (instruction == '0')
		{
				TIM_SetCompare1(TIM1,60);
				TIM_SetCompare4(TIM1,0);
				
				return(0);
		}
		else if (instruction == '1')
		{
				TIM_SetCompare1(TIM1,70);
				TIM_SetCompare4(TIM1,0);
				
				return(1);
		}
		else if (instruction == '2')
		{
				TIM_SetCompare1(TIM1,80);
				TIM_SetCompare4(TIM1,0);
				
				return(2);
		}
		else if (instruction == '3')
		{
				TIM_SetCompare1(TIM1,90);
				TIM_SetCompare4(TIM1,0);
				return(3);
		}
		else if (instruction == '4')
		{
				TIM_SetCompare1(TIM1,100);
				TIM_SetCompare4(TIM1,0);
				return(4);
		}
		//��ת�������λ 5-9
		else if (instruction == '5')
		{
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,0);
				DELAY_MS(500);
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,60);
				return(5);
		}
		else if (instruction == '6')
		{
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,0);
				DELAY_MS(500);
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,70);
				return(6);
		}
		else if (instruction == '7')
		{
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,0);
				DELAY_MS(500);
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,80);
				return(7);
		}
		else if (instruction == '8')
		{
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,0);
				DELAY_MS(500);
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,90);
				return(8);
		}
		else if (instruction == '9')
		{
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,0);
				DELAY_MS(500);
				TIM_SetCompare1(TIM1,0);
				TIM_SetCompare4(TIM1,100);
				return(9);
		}
		//���������Ч
		else
		{
				return('N');
		}
}