/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.h 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

//#include "DeviceManage/deviceManage.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

// USART 数据发送模块
#define ALARM_MAX_COUNT 2  // 最大允许警报次数
#define WAITINGSECOND 5000 // 等待回应报文的毫秒数 ms

  // USART 数据发送模块
  extern int8_t REPORT_DATA_STATUS;   // 数据发送状态,只有0或1
  extern uint32_t REPORT_TIM_CAPTURE; // 等待周期,1次表示经过了2000次记数,也就是200ms
  extern uint32_t ALARM_COUNT;        // 已触发警报次数,收到新数据后清空
  extern uint32_t ALARM_COUNT_TEST;   // 用于测试的报警次数, 不清零,用于统计

// 数据接收模块
#define RECESIZE 16
  extern int8_t REC_NUM;            // 之前已经读取了两个报文标识头 \n\n
  extern int8_t NEWLINE_FLAG;       // 标记是否为接收头
  extern int8_t RECEIVE_COMPLETED;  // 已完全获取数据
  extern int8_t REC_DATA[RECESIZE]; // 数据接收缓冲, 大小等于接受包的大小

  // LED1 工作灯状态
  extern uint32_t LED_PERIOD[2];

  void NMI_Handler(void);
  void HardFault_Handler(void);
  void MemManage_Handler(void);
  void BusFault_Handler(void);
  void UsageFault_Handler(void);
  void SVC_Handler(void);
  void DebugMon_Handler(void);
  void PendSV_Handler(void);
  void SysTick_Handler(void);

  //void ADC1_2_IRQHandler(void);

  //void USART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
