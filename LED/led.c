#include "led.h"

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
    GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);

    /* �ر�����led��	*/
    GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);

    /* �ر�����led��	*/
    GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
    
    // ����ָʾ��
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // 6Ϊ������
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    //GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Open_SignalLED()
{
    //DAC_SetChannel1Data(DAC_Align_12b_R,/*(2.0 / 3.3) * 4096*/4095);
    //DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);
    //LED_GREEN;
}

void Close_SignalLED()
{
    //LED2_OFF;;
    //DAC_SetChannel1Data(DAC_Align_12b_R,0);
    //DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);
}