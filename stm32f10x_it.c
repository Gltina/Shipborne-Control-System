/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    22-Jan-2022
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

#include "stm32f10x_it.h"
#include "LED/led.h"

// 数据发送模块
int8_t REPORT_DATA_STATUS = 0;   // 数据发送状态,只有0或1
uint32_t REPORT_TIM_CAPTURE = 0; // 等待周期,1次表示经过了2000次记数,也就是200ms
uint32_t ALARM_COUNT = 0;        // 已触发警报次数,收到新数据后清空
uint32_t ALARM_COUNT_TEST = 0;   // 用于测试的报警次数, 不清零,用于统计

// 数据接收模块
int8_t REC_NUM = 2; // 之前已经读取了两个报文标识头 \n\n
int8_t NEWLINE_FLAG = 0;
int8_t RECEIVE_COMPLETED = 0;
int8_t REC_DATA[RECESIZE] = {};

// LED1 工作灯状态
uint32_t LED_PERIOD[2] = {0, 0};

void TIM2_IRQHandler(void)
{
    /*
    *   设置为200ms
    *   2000 次是一个周期
    *   记数频率为 72000000/7200 = 10000次 , 即10000次为1s
    *   验证: 2000*7200 / 36000000 = 0.2s
    */

    // 发送完成后计时
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //发生计数器溢出更新中断
    {
        // 已发送报文数据
        if (REPORT_DATA_STATUS == 1)
        {
            REPORT_TIM_CAPTURE++; // 发送后累加周期, 1次表示经过了2000次记数,也就是200ms
        }

        // 控制灯光周期
        LED_PERIOD[0]++;
        LED_PERIOD[1]++;

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除中断标志位
    }
}

void USART1_IRQHandler(void)
{
    uint8_t status = 0;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        status = 1;
    }
    else if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET)
    {
        USART_ClearFlag(USART1, USART_FLAG_ORE);
        status = 2;
    }

    LED_WORKING_ON;
    // 收到即触发
    if (status == 1 || status == 2)
    {
        uint8_t result = USART_ReceiveData(USART1);
        
        // test code
//        if(result == '1')
//        {
//            TIM_SetCompare1(TIM3, 1700);
//        }
//        else if(result == '2')
//        {
//            TIM_SetCompare1(TIM3, 2100);
//        }
//        else if(result == '3')
//        {
//            TIM_SetCompare1(TIM3, 2400);
//        }
//        else if(result == '4')
//        {
//            TIM_SetCompare2(TIM3, 1300);
//        }
//        else if(result == '5')
//        {
//            TIM_SetCompare2(TIM3, 1800);
//        }
//        else if(result == '6')
//        {
//            TIM_SetCompare2(TIM3, 2300);
//        }
        
        // 已达成接收条件, 开始接收数据
        if (NEWLINE_FLAG == 2)
        {
            //LED_WORKING_ON;

            // 补充前两字节
            REC_DATA[0] = '\n';
            REC_DATA[1] = '\n';

            // 从第三位开始填充
            REC_DATA[REC_NUM++] = result;

            if (REC_NUM == RECESIZE)
            {
                /* 
             * 按照字节大小接收
             * 此时上位机数据接受完成
             * 接受的串口缓冲区应该为空
             * 
            */
                RECEIVE_COMPLETED = 1;
            }
            // 接受完成就退出当前函数
            return;
        }

        // 判断是否为接收头
        if (result == '\n')
        {
            NEWLINE_FLAG += 1;
        }
        else
        {
            NEWLINE_FLAG = 0;
        }


    }

    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET)
    {
        USART_ClearFlag(USART1, USART_FLAG_ORE);
        USART_ReceiveData(USART1);
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

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
