/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <limits.h>

#include "stm32f10x_it.h"

//#include "MPU6050/mpu6050.h"
//#include "WaterPress/waterpress.h"

#include "S201C/s201c.h"
#include "DeviceManage/deviceManage.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

// USART ���ݷ���ģ��
static int8_t REPORT_DATA_STATUS = 0;	// ���ݷ���״̬
static uint32_t REPORT_TIM_CAPTURE = 0; // �ȴ�����
static uint32_t REPORT_ERROR = 0;		// �Ƿ��Ѿ�����

void TIM2_IRQHandler(void)
{
	// ����Ϊ200ms
	// 2000 ����һ������
	// ����Ƶ��Ϊ 72000000/7200 = 10000�� , ��10000��Ϊ1s
	// ��֤: 2000*7200 / 36000000 = 0.2s

	// S201C
	static uint8_t S201C_STATUS = 0;	   //�洢����״̬��state=0��ʾδ���񵽵�һ�������أ�state=1��ʾ�Ѿ����񵽵�һ��������
	static uint32_t S201C_TIM_CAPTURE = 0; //�洢TIM2�����Ĵ����������

	

	if (TIM_GetITStatus(S201C_TIMX, TIM_IT_Update) != RESET) //������������������ж�
	{
		TIM_ClearITPendingBit(S201C_TIMX, TIM_IT_Update); //����жϱ�־λ

		// ˮ��������
		TIM_ClearITPendingBit(S201C_TIMX, S201C_IT_CCX);
		if (S201C_STATUS == 1) //�ڲ��񵽵�һ�������غ�
		{
			S201C_TIM_CAPTURE++; //���������һ
		}

		// ���ݷ���ģ��
		if (REPORT_DATA_STATUS == 0) // δ����
		{
			REPORT_DATA_STATUS = 1; // �����ѷ���
			REPORT_ERROR = 0;		// ��δ��������
			REPORT_TIM_CAPTURE = 0; // ���ͺ�ȴ������ÿ�

			LED_RGBOFF;

			// �رռ�ʱ, ���ⷢ��ʱ����������ٴ����
			TIM_Cmd(S201C_TIMX, DISABLE);
			report_device_data();
			TIM_Cmd(S201C_TIMX, ENABLE);
		}
		else if (REPORT_DATA_STATUS == 1) // ���ͺ����ʱ
		{
			REPORT_TIM_CAPTURE++; // ���ͺ��ۼ�����
		}
	}

	// ˮ��������
	if (TIM_GetITStatus(S201C_TIMX, S201C_IT_CCX) != RESET) //�������벶���ж�
	{
		TIM_ClearITPendingBit(S201C_TIMX, S201C_IT_CCX | TIM_IT_Update); //����жϱ�־λ
		if (S201C_STATUS == 0)											 //δ���񵽵�һ��������
		{
			S201C_STATUS = 1;			   //��1
			TIM_SetCounter(S201C_TIMX, 0); //������������
		}
		else if (S201C_STATUS == 1) //�Ѿ����񵽵�һ��������
		{
			S201C_STATUS = 0;
			uint32_t timecount = TIM_GetCapture4(S201C_TIMX) + S201C_TIM_CAPTURE * 2000; //��������������֮����ܼ���
			S201C_TIM_CAPTURE = 0;														 //�����������
			TIM_SetCounter(S201C_TIMX, 0);												 //���������

			float frequent_input = 10000.0 / timecount; //����Ƶ��
			float s = timecount / 10000.0;
			/*
            ���ݹ�ʽ: Ƶ��(f)=5*Q*s - 3, QΪ����,��λ��(L/min). sΪ��, ��λ����
            */
			//printf("%.3fL/min\r\n", frequent_input/5.0/s - 3.0);
            device_data.water_speed = frequent_input / 5.0 / s - 3.0;
		}
	}

	// ���ݷ���ģ��
	const uint32_t TIM_capture_M = 25;// 25*200 = 5000ms = 5s
    
	if (REPORT_TIM_CAPTURE < TIM_capture_M) 
	{}
	// ��ʱ����, ��ʱ�����źŽ����쳣�����. Ӧ�ò���Ӧ����ʩ
	else if (REPORT_TIM_CAPTURE < UINT32_MAX)
	{
		if (REPORT_ERROR == 0)
		{
			REPORT_ERROR = 1;
			//printf("Error: Data back delay!\r\n");
			LED_RED;
		}
	}
	// ����ȴ�ʱ�����(����һ��200msһ���жϼ���,��Ҫ��Լ27��Żᴥ������)
	else
	{
		REPORT_TIM_CAPTURE = TIM_capture_M;
	}
}

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

void USART1_IRQHandler(void)
{
	uint32_t status = 0;

	if (USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
		status = 1;
	}
	else if (USART_GetFlagStatus(USARTx, USART_FLAG_ORE) == SET)
	{
		USART_ClearFlag(USARTx, USART_FLAG_ORE);
		status = 2;
	}

	if (status == 1 || status == 2)
	{
		char result;
		result = USART_ReceiveData(USARTx);

		if (result == 'r')
		{
			LED_RED;
		}
		else if (result == 'g')
		{
			LED_GREEN;
		}
		else if (result == 'b')
		{
			LED_BLUE;
		}
		REPORT_DATA_STATUS = 0;
	}

	if (USART_GetFlagStatus(USARTx, USART_FLAG_ORE) == SET)
	{
		USART_ClearFlag(USARTx, USART_FLAG_ORE);
		USART_ReceiveData(USARTx);
	}
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
