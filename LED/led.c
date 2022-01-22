#include "led.h"

int LED_WORKING_ROOT = 1;
int LED_HIGHBEAM_ROOT = 1;
int LED_CAUTION_ROOT = 1;

void LED_Init(void)
{
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*����LED��ص�GPIO����ʱ��*/
    RCC_APB2PeriphClockCmd(LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);
    /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;

    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    /*������������Ϊ50MHz */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*���ÿ⺯������ʼ��GPIO*/
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);

    /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;

    /*���ÿ⺯������ʼ��GPIO*/
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);

    /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;

    /*���ÿ⺯������ʼ��GPIOF*/
    GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);

    /* �ر�����led��	*/
    GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);

    /* �ر�����led��	*/
    GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);

    /* �ر�����led��	*/
    GPIO_ResetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
}

void LED_period_control(uint8_t LED_index, uint32_t *period_count)
{
    uint32_t pc = *period_count;
    
    // ����LED_WORKING, ��1.2��, ��1.2��
    if (LED_index == 1)
    {
        if (pc < 3)
        {
            LED_WORKING_ON
        }
        else if (pc >= 3 && pc < 6)
        {
            LED_WORKING_OFF
        }
        else if(pc >= 6)
        {
            (*period_count) = 0;
        }
    }
    // ����LED_CAUTION , ������
    else if (LED_index == 3)
    {
        if (pc < 5)
        {
            LED_CAUTION_ON
        }
        else if (pc >= 5 && pc < 10)
        {
            LED_CAUTION_OFF
        }
        else if(pc >= 10)
        {
            (*period_count) = 0;
        }
    }
}