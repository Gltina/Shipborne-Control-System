#include "led.h"

int LED_WORKING_ROOT = 1;
int LED_HIGHBEAM_ROOT = 1;
int LED_CAUTION_ROOT = 1;

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
    GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);

    /* 关闭所有led灯	*/
    GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);

    /* 关闭所有led灯	*/
    GPIO_ResetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
}

void LED_period_control(uint8_t LED_index, uint32_t *period_count)
{
    uint32_t pc = *period_count;
    
    // 控制LED_WORKING, 亮1.2秒, 灭1.2秒
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
    // 控制LED_CAUTION , 亮两秒
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