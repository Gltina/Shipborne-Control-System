#include "waterPress.h"

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
    RCC_APB2PeriphClockCmd(WATERPRESS_CLOCK_RCC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = WATERPRESS_Pin0|WATERPRESS_Pin1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(WATERPRESS_GPIOx, &GPIO_InitStructure);
}

void Get_WaterPress_SensorData(float * s1, float * s2)
{
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5); //设定ADC规则组
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能软件启动
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;                                //等待转换结束
    *s1 =  ADC_GetConversionValue(ADC1)* 3.3 / 4096.0; //返回最后一次规则装换结果
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5); //设定ADC规则组
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能软件启动
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;                                //等待转换结束
    *s2 =  ADC_GetConversionValue(ADC1)* 3.3 / 4096.0; //返回最后一次规则装换结果
}

//float ADC2WaterDepth(uint16_t value)
//{
//    // voltage
//    float v =
//        (float)3.3 * value / 4096.0;

//    //printf("v=%0.3f\r\n", v);

//    //if(v < 0.5)
//    //    v = 0.5;

//    // water press (MPa)
//    //float wp
//    //    = fabs((v - 0.5)) / 5.0;

//    float wp = (v - 0.44) / 5.0;

//    // water depth (m), 1Mpa = 1000000Pa
//    float wd = wp * 1000000 / (1000 * 9.8);

//    // m->cm
//    return wd * 100;
//}

//float Get_WaterPress_Filtered()
//{
//    uint16_t adc_data[ADC_TIMES];

//    for (int8_t t = 0; t < ADC_TIMES; t++)
//    {
//        adc_data[t] = Get_WaterPress();
//        DELAY_MS(5);
//    }

//    uint16_t filtered_adc_value = filter(adc_data, ADC_TIMES);

//    return ADC2WaterDepth(filtered_adc_value);
//}

void Get_WaterDepth(float * water_depth0, float * water_depth1)
{
    Get_WaterPress_SensorData(water_depth0, water_depth1);
    
    // 数值装换, 将读取的电压值转换为水深
    
   
}







