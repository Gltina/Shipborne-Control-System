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

// USART ���ݷ���ģ��
#define ALARM_MAX_COUNT 2  // �������������
#define WAITINGSECOND 5000 // �ȴ���Ӧ���ĵĺ����� ms

static int8_t REPORT_DATA_STATUS = 0;   // ���ݷ���״̬,ֻ��0��1
static uint32_t REPORT_TIM_CAPTURE = 0; // �ȴ�����,1�α�ʾ������2000�μ���,Ҳ����200ms
static uint32_t ALARM_COUNT = 0;        // �Ѵ�����������,�յ������ݺ����
static uint32_t ALARM_COUNT_TEST = 0;   // ���ڲ��Եı�������, ������,����ͳ��

// ���ݽ���ģ��
#define RECESIZE 16

void TIM2_IRQHandler(void)
{
    /*
    *   ����Ϊ200ms
    *   2000 ����һ������
    *   ����Ƶ��Ϊ 72000000/7200 = 10000�� , ��10000��Ϊ1s
    *   ��֤: 2000*7200 / 36000000 = 0.2s
    */

    // ���ȼ���Ƿ�ʱ
    if (TIM_GetITStatus(SENDING_TIMX, TIM_IT_Update) != RESET)
    {
        // ��ʱ����, ��ʱ�����źŽ����쳣�����. Ӧ�ò���Ӧ����ʩ
        if ((REPORT_TIM_CAPTURE * 200) > WAITINGSECOND) // ��$(REPORT_TIM_CAPTURE)תΪ����
        {
            ALARM_COUNT++;
            ALARM_COUNT_TEST++;

            if (ALARM_COUNT == ALARM_MAX_COUNT)
            {
                LED_RED; // �������
                while (1)
                {
                }
            }
            else // ��1��δ��ʱ�յ�����
            {
                REPORT_DATA_STATUS = 0; // �ٴγ��Է���
            }
        }
        else if (REPORT_TIM_CAPTURE == UINT32_MAX) // ����ȴ�ʱ�����(����һ��200msһ���жϼ���,��Ҫ��Լ27��Żᴥ������)
        {
            REPORT_TIM_CAPTURE = (WAITINGSECOND / 200);
        }
    }

    // ���뷢��һ�㱨������
    if (TIM_GetITStatus(SENDING_TIMX, TIM_IT_Update) != RESET) //������������������ж�
    {
        if (REPORT_DATA_STATUS == 0) // δ����,�������ͱ���
        {
            LED_RGBOFF;

            TIM_Cmd(SENDING_TIMX, DISABLE);  // �رռ�ʱ, ���ⷢ��ʱ����������ٴ����
            TIM_SetCounter(SENDING_TIMX, 0); // ���������

            // ����һ�㱨��
            read_device_data();
            encap_msg_sending(0, NULL);
            // end

            TIM_ClearITPendingBit(SENDING_TIMX, TIM_IT_Update); //����жϱ�־λ, ���⿪����ʱ�����ٴν��붨ʱ��
            TIM_Cmd(SENDING_TIMX, ENABLE);                      // ������ʱ

            REPORT_DATA_STATUS = 1; // ��������ѷ���
            REPORT_TIM_CAPTURE = 0; // ���ͺ�ȴ������ÿ�, ���ڷ��ͱ��ĺ�����
        }
        else if (REPORT_DATA_STATUS == 1)
        {
            REPORT_TIM_CAPTURE++; // ���ͺ��ۼ�����, 1�α�ʾ������2000�μ���,Ҳ����200ms
        }
    }

    TIM_ClearITPendingBit(SENDING_TIMX, TIM_IT_Update); //����жϱ�־λ
}

void USART1_IRQHandler(void)
{
    static int8_t newline_flag = 0;
    static int8_t rec_num = 2; // ֮ǰ�Ѿ���ȡ���������ı�ʶͷ \n\n
    static int8_t rec_data[RECESIZE];

    // �����ճ���
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

    // �յ�������
    if (status == 1 || status == 2)
    {
        uint8_t result = USART_ReceiveData(USARTx);

        // �Ѵ�ɽ�������, ��ʼ��������
        if (newline_flag == 2)
        {
            LED_RGBOFF;

            // ����ǰ���ֽ�
            rec_data[0] = '\n';
            rec_data[1] = '\n';

            // �ӵ���λ��ʼ���
            rec_data[rec_num++] = result;

            // �����ֽڴ�С����
            if (rec_num == RECESIZE)
            {
                receiving_package_s receiving_package;
                memcpy(&receiving_package, rec_data, sizeof(rec_data));
                // ��ʱ��λ�����ݽ������
                // ���ܵĴ��ڻ�����Ӧ��Ϊ��

                // Ӧ���µ�״ָ̬ʾ
                apply_control_signal(&receiving_package.device_status);

#ifdef SHOW_TIME1
                // ����ӷ��ͱ��ĵ���Ӧ��ȫ���յĺ�ʱ
                char send_str[50];
                uint32_t receive_delay = TIM_GetCapture4(SENDING_TIMX) + REPORT_TIM_CAPTURE * 2000;
                sprintf(send_str, "ID:%d,Length:%d,T1=%.3fs (alarm count:%d)",
                        receiving_package.device_ID, receiving_package.data_length,
                        receive_delay / 10000.0, ALARM_COUNT_TEST);
                encap_msg_sending(1, send_str);
                memset(send_str, '\0', strlen(send_str));
#endif //SHOW_TIME1

                // ������
                memset((void *)rec_data, '\0', RECESIZE); // ��ս�������
                rec_num = 2;                              // ���ý�������
                newline_flag = 0;                         // ���ý���ͷ��ʶ

                // д����ʱ�����������ͱ��Ķ�ʱ��
                while (TIM_GetITStatus(SENDING_TIMX, TIM_IT_Update) == RESET)
                {
                    // �ȴ�SENDING_TIMX���ж�ʱ��, ���������޸�״̬ʱ����ȡ
                }
                REPORT_DATA_STATUS = 0; // ״̬��Ϊ�������ͱ���, ���������仯
                ALARM_COUNT = 0;        // ȡ����������
            }
            // ������ɾ��˳���ǰ����
            return;
        }

        // �ж��Ƿ�Ϊ����ͷ
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
