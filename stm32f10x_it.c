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

// USART 数据发送模块
static int8_t REPORT_DATA_STATUS = 0;	// 数据发送状态
static uint32_t REPORT_TIM_CAPTURE = 0; // 等待周期
static uint32_t REPORT_ERROR = 0;		// 是否已经报警

void TIM2_IRQHandler(void)
{
	// 设置为200ms
	// 2000 次是一个周期
	// 记数频率为 72000000/7200 = 10000次 , 即10000次为1s
	// 验证: 2000*7200 / 36000000 = 0.2s

	// S201C
	static uint8_t S201C_STATUS = 0;	   //存储捕获状态，state=0表示未捕获到第一个上升沿，state=1表示已经捕获到第一个上升沿
	static uint32_t S201C_TIM_CAPTURE = 0; //存储TIM2计数寄存器溢出次数

	

	if (TIM_GetITStatus(S201C_TIMX, TIM_IT_Update) != RESET) //发生计数器溢出更新中断
	{
		TIM_ClearITPendingBit(S201C_TIMX, TIM_IT_Update); //清除中断标志位

		// 水流计速器
		TIM_ClearITPendingBit(S201C_TIMX, S201C_IT_CCX);
		if (S201C_STATUS == 1) //在捕获到第一个上升沿后
		{
			S201C_TIM_CAPTURE++; //溢出次数加一
		}

		// 数据发送模块
		if (REPORT_DATA_STATUS == 0) // 未发送
		{
			REPORT_DATA_STATUS = 1; // 数据已发送
			REPORT_ERROR = 0;		// 还未触发报警
			REPORT_TIM_CAPTURE = 0; // 发送后等待周期置空

			LED_RGBOFF;

			// 关闭计时, 避免发送时间过长导致再次溢出
			TIM_Cmd(S201C_TIMX, DISABLE);
			report_device_data();
			TIM_Cmd(S201C_TIMX, ENABLE);
		}
		else if (REPORT_DATA_STATUS == 1) // 发送后仅计时
		{
			REPORT_TIM_CAPTURE++; // 发送后累加周期
		}
	}

	// 水流计速器
	if (TIM_GetITStatus(S201C_TIMX, S201C_IT_CCX) != RESET) //产生输入捕获中断
	{
		TIM_ClearITPendingBit(S201C_TIMX, S201C_IT_CCX | TIM_IT_Update); //清除中断标志位
		if (S201C_STATUS == 0)											 //未捕获到第一个上升沿
		{
			S201C_STATUS = 1;			   //置1
			TIM_SetCounter(S201C_TIMX, 0); //将计数器清零
		}
		else if (S201C_STATUS == 1) //已经捕获到第一个上升沿
		{
			S201C_STATUS = 0;
			uint32_t timecount = TIM_GetCapture4(S201C_TIMX) + S201C_TIM_CAPTURE * 2000; //计算两个上升沿之间的总计数
			S201C_TIM_CAPTURE = 0;														 //清零溢出次数
			TIM_SetCounter(S201C_TIMX, 0);												 //清零计数器

			float frequent_input = 10000.0 / timecount; //计算频率
			float s = timecount / 10000.0;
			/*
            根据公式: 频率(f)=5*Q*s - 3, Q为流量,单位是(L/min). s为秒, 单位是秒
            */
			//printf("%.3fL/min\r\n", frequent_input/5.0/s - 3.0);
            device_data.water_speed = frequent_input / 5.0 / s - 3.0;
		}
	}

	// 数据发送模块
	const uint32_t TIM_capture_M = 25;// 25*200 = 5000ms = 5s
    
	if (REPORT_TIM_CAPTURE < TIM_capture_M) 
	{}
	// 超时处理, 此时出现信号接收异常的情况. 应该采用应急措施
	else if (REPORT_TIM_CAPTURE < UINT32_MAX)
	{
		if (REPORT_ERROR == 0)
		{
			REPORT_ERROR = 1;
			//printf("Error: Data back delay!\r\n");
			LED_RED;
		}
	}
	// 避免等待时间过长(按照一个200ms一次中断计算,需要大约27年才会触发到这)
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
