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
#define ALARM_MAX_COUNT 2  // 最大允许警报次数
#define WAITINGSECOND 5000 // 等待回应报文的毫秒数 ms

static int8_t REPORT_DATA_STATUS = 0;   // 数据发送状态,只有0或1
static uint32_t REPORT_TIM_CAPTURE = 0; // 等待周期,1次表示经过了2000次记数,也就是200ms
static uint32_t ALARM_COUNT = 0;        // 已触发警报次数,收到新数据后清空
static uint32_t ALARM_COUNT_TEST = 0;   // 用于测试的报警次数, 不清零,用于统计

// 数据接收模块
#define RECESIZE 16

void TIM2_IRQHandler(void)
{
    /*
    *   设置为200ms
    *   2000 次是一个周期
    *   记数频率为 72000000/7200 = 10000次 , 即10000次为1s
    *   验证: 2000*7200 / 36000000 = 0.2s
    */

    // 首先检测是否超时
    if (TIM_GetITStatus(SENDING_TIMX, TIM_IT_Update) != RESET)
    {
        // 超时处理, 此时出现信号接收异常的情况. 应该采用应急措施
        if ((REPORT_TIM_CAPTURE * 200) > WAITINGSECOND) // 将$(REPORT_TIM_CAPTURE)转为毫秒
        {
            ALARM_COUNT++;
            ALARM_COUNT_TEST++;

            if (ALARM_COUNT == ALARM_MAX_COUNT)
            {
                LED_RED; // 紧急情况
                while (1)
                {
                }
            }
            else // 第1此未按时收到数据
            {
                REPORT_DATA_STATUS = 0; // 再次尝试发送
            }
        }
        else if (REPORT_TIM_CAPTURE == UINT32_MAX) // 避免等待时间过长(按照一个200ms一次中断计算,需要大约27年才会触发到这)
        {
            REPORT_TIM_CAPTURE = (WAITINGSECOND / 200);
        }
    }

    // 进入发送一般报文流程
    if (TIM_GetITStatus(SENDING_TIMX, TIM_IT_Update) != RESET) //发生计数器溢出更新中断
    {
        if (REPORT_DATA_STATUS == 0) // 未发送,触发发送报文
        {
            LED_RGBOFF;

            TIM_Cmd(SENDING_TIMX, DISABLE);  // 关闭计时, 避免发送时间过长导致再次溢出
            TIM_SetCounter(SENDING_TIMX, 0); // 清除计数器

            // 发送一般报文
            read_device_data();
            encap_msg_sending(0, NULL);
            // end

            TIM_ClearITPendingBit(SENDING_TIMX, TIM_IT_Update); //清除中断标志位, 避免开启定时器后再次进入定时器
            TIM_Cmd(SENDING_TIMX, ENABLE);                      // 开启计时

            REPORT_DATA_STATUS = 1; // 标记数据已发送
            REPORT_TIM_CAPTURE = 0; // 发送后等待周期置空, 仅在发送报文后置零
        }
        else if (REPORT_DATA_STATUS == 1)
        {
            REPORT_TIM_CAPTURE++; // 发送后累加周期, 1次表示经过了2000次记数,也就是200ms
        }
    }

    TIM_ClearITPendingBit(SENDING_TIMX, TIM_IT_Update); //清除中断标志位
}

void USART1_IRQHandler(void)
{
    static int8_t newline_flag = 0;
    static int8_t rec_num = 2; // 之前已经读取了两个报文标识头 \n\n
    static int8_t rec_data[RECESIZE];

    // 检查接收长度
    if (RECESIZE != sizeof(receiving_package_s))
    {
        LED_RGBOFF;
        LED_RED;
        //while (1){}
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
        uint8_t result = USART_ReceiveData(USARTx);

        // 已达成接收条件, 开始接收数据
        if (newline_flag == 2)
        {
            LED_RGBOFF;

            // 补充前两字节
            rec_data[0] = '\n';
            rec_data[1] = '\n';

            // 从第三位开始填充
            rec_data[rec_num++] = result;

            // 按照字节大小接收
            if (rec_num == RECESIZE)
            {
                receiving_package_s receiving_package;
                memcpy(&receiving_package, rec_data, sizeof(rec_data));
                // 此时上位机数据接受完成
                // 接受的串口缓冲区应该为空

                // 应用新的状态指示
                apply_control_signal(&receiving_package.device_status);

#ifdef SHOW_TIME1
                // 计算从发送报文到回应完全接收的耗时
                char send_str[50];
                uint32_t receive_delay = TIM_GetCapture4(SENDING_TIMX) + REPORT_TIM_CAPTURE * 2000;
                sprintf(send_str, "ID:%d,Length:%d,T1=%.3fs (alarm count:%d)",
                        receiving_package.device_ID, receiving_package.data_length,
                        receive_delay / 10000.0, ALARM_COUNT_TEST);
                encap_msg_sending(1, send_str);
                memset(send_str, '\0', strlen(send_str));
#endif //SHOW_TIME1

                // 清理工作
                memset((void *)rec_data, '\0', RECESIZE); // 清空接收区域
                rec_num = 2;                              // 重置接收数量
                newline_flag = 0;                         // 重置接收头标识

                // 写数据时避免遇到发送报文定时器
                while (TIM_GetITStatus(SENDING_TIMX, TIM_IT_Update) == RESET)
                {
                    // 等待SENDING_TIMX非中断时间, 尽量避免修改状态时被读取
                }
                REPORT_DATA_STATUS = 0; // 状态改为继续发送报告, 放置在最后变化
                ALARM_COUNT = 0;        // 取消警报次数
            }
            // 接受完成就退出当前函数
            return;
        }

        // 判断是否为接收头
        if (result == '\n')
        {
            newline_flag += 1;
        }
        else
        {
            newline_flag = 0;
        }
    }

    if (USART_GetFlagStatus(USARTx, USART_FLAG_ORE) == SET)
    {
        USART_ClearFlag(USARTx, USART_FLAG_ORE);
        USART_ReceiveData(USARTx);
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
