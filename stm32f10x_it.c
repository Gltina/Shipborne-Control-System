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
#include "stm32f10x_it.h"
#include "DeviceManage/deviceManage.h"

#define RECESIZE 14

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

// From deviceManage.h (from deviceManage.c)
extern device_data_s device_data;
//extern device_status_s device_status;

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
    
    // 200ms 正常中断
	if (TIM_GetITStatus(S201C_TIMX, TIM_IT_Update) != RESET) //发生计数器溢出更新中断
	{
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

			LED_RGBOFF;

			TIM_Cmd(S201C_TIMX, DISABLE);   // 关闭计时, 避免发送时间过长导致再次溢出
			//report_device_data();
            read_device_data();
            encap_msg_sending();
			TIM_Cmd(S201C_TIMX, ENABLE);    // 开启计时
            
            REPORT_TIM_CAPTURE = 0; // 发送后等待周期置空
		}
		else if (REPORT_DATA_STATUS == 1)
		{
			REPORT_TIM_CAPTURE++; // 发送后累加周期
		}
	}

	// 水流计速器中断捕获
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
			device_data.WaterSpeed = frequent_input / 5.0 / s - 3.0;
		}
	}
    
    if (TIM_GetITStatus(S201C_TIMX, TIM_IT_Update) != RESET)
    {
        // 监测是否按时收到数据
        const uint32_t waiting_second = 5; // 设置等待的秒数
        const uint32_t TIM_Capture_Threshold = (waiting_second * 1000) / 200; // x * 1000ms / 200ms
        
        // 超时处理, 此时出现信号接收异常的情况. 应该采用应急措施
        if (REPORT_TIM_CAPTURE > TIM_Capture_Threshold)
        {
            
            // TODO
            // 应用紧急处理
            
            
            // 测试用, 仅显示一次错误
            if (REPORT_ERROR == 0)
            {
                REPORT_ERROR = 1;
                //printf("Error: Data back delay!\r\n");
                LED_RED;
            }
        }
        else if(REPORT_TIM_CAPTURE == UINT32_MAX)// 避免等待时间过长(按照一个200ms一次中断计算,需要大约27年才会触发到这)
        {
            REPORT_TIM_CAPTURE = TIM_Capture_Threshold;
        }
    }
	
    TIM_ClearITPendingBit(S201C_TIMX, TIM_IT_Update); //清除中断标志位
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
    static int8_t newline_flag = 0;
    static int8_t rec_num = 2;
    static int8_t rec_data[RECESIZE];
    
    if(sizeof(rec_data[RECESIZE]) != sizeof(receiving_package_s))
    {
        LED_RGBOFF;
        LED_RED;
    }
    
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
    
    // 收到即触发
	if (status == 1 || status == 2)
	{
        LED_RGBOFF;
        
		uint8_t result = USART_ReceiveData(USARTx);
        
        // 已达成接收条件, 开始接收数据
        if(newline_flag == 2)
        {
            // 补充前两字节
            rec_data[0] = '\n';
            rec_data[1] = '\n';
            
            // 从第三位开始填充
            rec_data[rec_num++] = result;
            
            // 按照字节大小接收
            if(rec_num == RECESIZE)
            {
                receiving_package_s receiving_package;
                memcpy(&receiving_package, rec_data, sizeof(rec_data));
                //printf("%d %d#", receiving_package.data_length,receiving_package.device_ID );
                // 此时上位机数据接受完成
                // 接受的串口缓冲区应该为空
                
                // 应用新的状态指示
                apply_control_signal(&receiving_package.device_status);
                
                // 清理工作
                memset((void *)rec_data,0,sizeof(rec_data));// 清空接收区域
                rec_num = 2;                                // 重置接收数量
                newline_flag = 0;                           // 重置接收头标识
                
                REPORT_DATA_STATUS = 0;                     // 状态改为继续发送报告, 放置在最后变化
                
                //uint32_t receive_delay = TIM_GetCapture4(S201C_TIMX) + REPORT_TIM_CAPTURE * 2000;
                //printf("%.4f got#", receive_delay/ 10000.0);
            }
        }
        
        // 判断是否为接收头
        if(result == '\n')
        {
            newline_flag ++;
        }
        /*
        // 测试代码
//		if (result == 'r')
//		{
//			LED_RED;
//            WaterTank_IN_Open();
//		}
//		else if (result == 'g')
//		{
//			LED_GREEN;
//            WaterTank_IN_Close();
//		}
//		else if (result == 'b')
//		{
//			LED_BLUE;
//            WaterTank_OUT_Open();
//        }
//        else if(result == 'a')
//        {
//            LED_RGBOFF;
//            WaterTank_OUT_Close();
//        }
//        else if(result == 'q')
//        {
//            LED_YELLOW;
//            WaterTank_OUT_Close();
//            WaterTank_IN_Close();
//        }
        // end 测试代码
        */
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
