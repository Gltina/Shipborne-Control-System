#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

// LED�ĵ�Դ����, ��û�򿪴˿���, �޷�ʹ�øߵ�ƽ����
extern int LED_WORKING_ROOT;
extern int LED_HIGHBEAM_ROOT;
extern int LED_CAUTION_ROOT;

// һ���������
// LED1 Ϊ����״ָ̬ʾ��(һ����ɫ��)
// LED2 Ϊǰ�յ�(������ɫ��)
// LED3 Ϊ��ʾ��(������ɫ��)

// LED1 Ϊ����״ָ̬ʾ��(һ����ɫ��), �����޸�״̬
#define LED1_GPIO_PORT GPIOB
#define LED1_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED1_GPIO_PIN GPIO_Pin_0

// LED2 Ϊǰ�յ�(������ɫ��)
#define LED2_GPIO_PORT GPIOB
#define LED2_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED2_GPIO_PIN GPIO_Pin_1

// LED3 Ϊ��ʾ��(������ɫ��), �����޸�״̬
#define LED3_GPIO_PORT GPIOB
#define LED3_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED3_GPIO_PIN GPIO_Pin_5

/* ʹ�ñ�׼�Ĺ̼������IO*/
#define LED_WORKING_OFF GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
#define LED_WORKING_ON         \
    if (LED_WORKING_ROOT == 1) \
    {GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);}

#define LED_HIGHBEAM_OFF GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);
#define LED_HIGHBEAM_ON         \
    if (LED_HIGHBEAM_ROOT == 1) \
    {GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);}

#define LED_CAUTION_OFF GPIO_ResetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
#define LED_CAUTION_ON         \
    if (LED_CAUTION_ROOT == 1) \
    {GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);}

void LED_Init(void);

void LED_period_control(uint8_t LED_index, uint32_t *period_count);

#endif /* __LED_H */
