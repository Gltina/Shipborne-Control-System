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
    
    // 200ms �����ж�
	if (TIM_GetITStatus(S201C_TIMX, TIM_IT_Update) != RESET) //������������������ж�
	{
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

			LED_RGBOFF;

			TIM_Cmd(S201C_TIMX, DISABLE);   // �رռ�ʱ, ���ⷢ��ʱ����������ٴ����
			//report_device_data();
            read_device_data();
            encap_msg_sending();
			TIM_Cmd(S201C_TIMX, ENABLE);    // ������ʱ
            
            REPORT_TIM_CAPTURE = 0; // ���ͺ�ȴ������ÿ�
		}
		else if (REPORT_DATA_STATUS == 1)
		{
			REPORT_TIM_CAPTURE++; // ���ͺ��ۼ�����
		}
	}

	// ˮ���������жϲ���
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
			device_data.WaterSpeed = frequent_input / 5.0 / s - 3.0;
		}
	}
    
    if (TIM_GetITStatus(S201C_TIMX, TIM_IT_Update) != RESET)
    {
        // ����Ƿ�ʱ�յ�����
        const uint32_t waiting_second = 5; // ���õȴ�������
        const uint32_t TIM_Capture_Threshold = (waiting_second * 1000) / 200; // x * 1000ms / 200ms
        
        // ��ʱ����, ��ʱ�����źŽ����쳣�����. Ӧ�ò���Ӧ����ʩ
        if (REPORT_TIM_CAPTURE > TIM_Capture_Threshold)
        {
            
            // TODO
            // Ӧ�ý�������
            
            
            // ������, ����ʾһ�δ���
            if (REPORT_ERROR == 0)
            {
                REPORT_ERROR = 1;
                //printf("Error: Data back delay!\r\n");
                LED_RED;
            }
        }
        else if(REPORT_TIM_CAPTURE == UINT32_MAX)// ����ȴ�ʱ�����(����һ��200msһ���жϼ���,��Ҫ��Լ27��Żᴥ������)
        {
            REPORT_TIM_CAPTURE = TIM_Capture_Threshold;
        }
    }
	
    TIM_ClearITPendingBit(S201C_TIMX, TIM_IT_Update); //����жϱ�־λ
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
    
    // �յ�������
	if (status == 1 || status == 2)
	{
        LED_RGBOFF;
        
		uint8_t result = USART_ReceiveData(USARTx);
        
        // �Ѵ�ɽ�������, ��ʼ��������
        if(newline_flag == 2)
        {
            // ����ǰ���ֽ�
            rec_data[0] = '\n';
            rec_data[1] = '\n';
            
            // �ӵ���λ��ʼ���
            rec_data[rec_num++] = result;
            
            // �����ֽڴ�С����
            if(rec_num == RECESIZE)
            {
                receiving_package_s receiving_package;
                memcpy(&receiving_package, rec_data, sizeof(rec_data));
                //printf("%d %d#", receiving_package.data_length,receiving_package.device_ID );
                // ��ʱ��λ�����ݽ������
                // ���ܵĴ��ڻ�����Ӧ��Ϊ��
                
                // Ӧ���µ�״ָ̬ʾ
                apply_control_signal(&receiving_package.device_status);
                
                // ������
                memset((void *)rec_data,0,sizeof(rec_data));// ��ս�������
                rec_num = 2;                                // ���ý�������
                newline_flag = 0;                           // ���ý���ͷ��ʶ
                
                REPORT_DATA_STATUS = 0;                     // ״̬��Ϊ�������ͱ���, ���������仯
                
                //uint32_t receive_delay = TIM_GetCapture4(S201C_TIMX) + REPORT_TIM_CAPTURE * 2000;
                //printf("%.4f got#", receive_delay/ 10000.0);
            }
        }
        
        // �ж��Ƿ�Ϊ����ͷ
        if(result == '\n')
        {
            newline_flag ++;
        }
        /*
        // ���Դ���
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
        // end ���Դ���
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
