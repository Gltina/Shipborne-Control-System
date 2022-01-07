#include "waterpress.h"

//void ADC_NVIC_Config(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

//    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

//    NVIC_Init(&NVIC_InitStructure);
// }

void WaterPress_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //����Ϊģ������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  //ADC ����ģʽ:����ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       //AD ��ͨ��ģʽ;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                 //AD ����ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ת��������������ⲿ��������
    
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //ADC �����Ҷ���;
    ADC_InitStructure.ADC_NbrOfChannel = 1;                             //˳����й���ת���� ADC ͨ������Ŀ 1
    ADC_Init(ADC1, &ADC_InitStructure);
    
    //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); // ADC ת�����������жϣ����жϷ�������ж�ȡת��ֵ
    //ADC_NVIC_Config();
    
    //ADC_DeInit(ADC1); //��λʱ��
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); //����ADCʱ�� һ�㲻����14MHZ ���򾫶Ȳ�׼ȷ  72MHZ/6=12MHZ
    
    ADC_Cmd(ADC1, ENABLE); //ʹ��ADC1

    //����ָ���� ADC ��У׼�Ĵ���
    ADC_StartCalibration(ADC1);
    //�ȴ���λУ׼����
    while (ADC_GetResetCalibrationStatus(ADC1));
    
    // ADC ��ʼУ׼
    ADC_ResetCalibration(ADC1);
    //�ȴ�У AD ׼����
    while (ADC_GetCalibrationStatus(ADC1)); 
    
    //ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5); //�趨ADC������
    
    //ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ���������
}

uint16_t Get_WaterPress()
{
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5); //�趨ADC������
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ���������

    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;                                //�ȴ�ת������
    return ADC_GetConversionValue(ADC1); //�������һ�ι���װ�����
}

float ADC2WaterDepth(uint16_t value)
{
    // voltage
    float v =
        (float)3.3 * value/4096.0;
    
    //printf("v=%0.3f\r\n", v);
    
    //if(v < 0.5)
    //    v = 0.5;
    
    // water press (MPa)
    //float wp
    //    = fabs((v - 0.5)) / 5.0;
    
    float wp
        = (v - 0.44) / 5.0;
    
    // water depth (m), 1Mpa = 1000000Pa
    float wd
        = wp * 1000000 / (1000 * 9.8);
    
    // m->cm
    return wd * 100;
}

float Get_WaterPress_Filtered()
{    
    uint16_t adc_data[ADC_TIMES];
    
    for (int8_t t = 0; t < ADC_TIMES; t++)
    {
        adc_data[t] = Get_WaterPress();
        DELAY_MS(5);
    }
    
    uint16_t filtered_adc_value 
        = filter(adc_data, ADC_TIMES);
    
    return ADC2WaterDepth(filtered_adc_value);
}

