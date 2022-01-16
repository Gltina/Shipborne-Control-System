#include "waterTank.h"

void WaterTank_Init()
{
    // ��ʼ��ˮ�䷧�� 
    {
        RCC_APB2PeriphClockCmd(WATERTANK_DOOR_CLOCK_RCC, ENABLE);
        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Pin = WATERTANK_DOOR_IN_Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(WATERTANK_DOOR_GPIOx, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = WATERTANK_DOOR_OUT_Pin;
        GPIO_Init(WATERTANK_DOOR_GPIOx, &GPIO_InitStructure);

        WaterTank_IN_Close();WaterTank_OUT_Close();
    }

    // ��ʼ��ˮ��Һ��GPIO
    {
        RCC_APB2PeriphClockCmd(WATERTANK_LEVEL_CLOCK_RCC, ENABLE);
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = WATERTANK_LEVEL0_Pin | WATERTANK_LEVEL1_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIOA, &GPIO_InitStructure); 
    }
}

void WaterTank_IN_Open()
{
    GPIO_SetBits(WATERTANK_DOOR_GPIOx, WATERTANK_DOOR_IN_Pin);
}

void WaterTank_IN_Close()
{
    GPIO_ResetBits(WATERTANK_DOOR_GPIOx, WATERTANK_DOOR_IN_Pin);
}

void WaterTank_OUT_Open()
{
    GPIO_SetBits(WATERTANK_DOOR_GPIOx, WATERTANK_DOOR_OUT_Pin);
}
void WaterTank_OUT_Close()
{
    GPIO_ResetBits(WATERTANK_DOOR_GPIOx, WATERTANK_DOOR_OUT_Pin);
}

void set_waterTank_IN_status(uint8_t status)
{
    if (status == 1)
    {
        WaterTank_IN_Open();
    }
    else if (status == 0)
    {
        WaterTank_IN_Close();
    }
}

void set_waterTank_OUT_status(uint8_t status)
{
    if (status == 1)
    {
        WaterTank_OUT_Open();
    }
    else if (status == 0)
    {
        WaterTank_OUT_Close();
    }
}

void Get_WaterTank_SensorData(float * s1, float *s2)
{
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5); //�趨ADC������
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);                                      //ʹ���������
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;                                                          //�ȴ�ת������
    *s1 = ADC_GetConversionValue(ADC1) * 3.30 / 4096.0; //�������һ�ι���װ�����

    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5); //�趨ADC������
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;                                                          //�ȴ�ת������
    *s2 = ADC_GetConversionValue(ADC1) * 3.30 / 4096.0; //�������һ�ι���װ�����
}

void Get_WaterLevel_WaterTank(float *water_level_0, float *water_level_1)
{
    Get_WaterTank_SensorData(water_level_0,water_level_1);
    // ��ֵװ��, ����ȡ�ĵ�ѹֵת��Ϊˮ��
}
