#include "led.h"

void LED_Init(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启LED相关的GPIO外设时钟*/
    RCC_APB2PeriphClockCmd(LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);
    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;

    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    /*设置引脚速率为50MHz */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*调用库函数，初始化GPIO*/
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);

    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;

    /*调用库函数，初始化GPIO*/
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);

    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;

    /*调用库函数，初始化GPIOF*/
    GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);

    /* 关闭所有led灯	*/
    GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);

    /* 关闭所有led灯	*/
    GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);

    /* 关闭所有led灯	*/
    GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
    
    
    // 设置指示灯
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // 6为测试用
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    DAC_InitTypeDef DAC_InitStructure;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None; //不使用波形发生器
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;   //不使用 DAC 输出缓冲
    //DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits11_0;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_1, ENABLE);
}

void Open_SignalLED()
{
    DAC_SetChannel1Data(DAC_Align_12b_R,/*(2.0 / 3.3) * 4096*/4095);
    DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);
}

void Close_SignalLED()
{
    DAC_SetChannel1Data(DAC_Align_12b_R,0);
    DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);
}