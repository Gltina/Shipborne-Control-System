#include "srf05.h"

//�Դ˷���һ����ʱ�����Լ���չ4ͨ���ɼ�
//ʵ�ʷ�Χ(2cm-450cm),����(1cm-500cm)*58us����Ĵ���ֵ����������ֵ
//������������ն�ʱ������ȷ���ݾͲ������500*58=29000<65536
//�������֤���״��޷�Ӧ,��ʱ��������ȥ65536*1us=65ms ��Լ����Ϊ�Ƽ�����60ms
//Ҳ����65ms����ɨ���״ǰ�����β������,����������жϴ����״ﳬʱ,�״���û�з�Ӧ������λ�ˣ���֤׼ȷ��
//�쳣ֵ�ƶ�����δ�ƶ��������ϴ���Чֵ

u16 TIM3_Ultrasonic_VAL;

u32 TIM3_Ultrasonic_FLAG1, Ultrasonic_Time1, Ultrasonic_Tim_old1;

//uint16_t Get_Distance(float * distance_cm)
//{
//    *distance_cm = 0;
//    
//    DELAY_US(500);

//    SRF05_Trig();
//    
//    *distance_cm = (float)Ultrasonic_Time1 / 58.0;
//    
//    return 0;
//}

void SRF05_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(SRF05_CLOCK_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = Trig_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOx, Trig_Pin);
    
    GPIO_InitStructure.GPIO_Pin = Echo_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOx, Echo_Pin);
    
    TIM3_SRF05_Init(0xffff, 72 - 1);
}

//Tout�����ʱ�䣩=��ARR+1)(PSC+1)/Tclk
//����Ƶ��=Tclk/(PSC+1)=72m/72=1m
void TIM3_SRF05_Init(u16 arr, u16 psc)
{
    //printf("xxxx=%d\r\n",arr);
    //GPIO_InitTypeDef GPIO_InitStructure;
    
    //TIM_ICInitTypeDef TIM_ICInitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;

    //TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); //����ȱʡֵ,��һ����ü��Ϸ�ֹ�ŵ����ڳ�ʼ���������
    //TIM_OCStructInit(&TIM_OCInitStructure);        //����ȱʡֵ,��һ����ü��Ϸ�ֹ�ŵ����ڳ�ʼ���������
    //TIM_ICStructInit(&TIM_ICInitStructure); //����ȱʡֵ,��һ����ü��Ϸ�ֹ�ŵ����ڳ�ʼ���������

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    //RCC_APB2PeriphClockCmd(SRF05_CLOCK_RCC, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

//    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
//    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
//    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
//    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//    TIM_ICInitStructure.TIM_ICFilter = 0x01;
//    TIM_ICInit(TIM3, &TIM_ICInitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

//    TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
//    TIM_Cmd(TIM3, DISABLE);
}

//void SRF05_Trig(void)
//{
//    GPIO_SetBits(GPIOx, Trig_Pin);
//    DELAY_US(10);
//    GPIO_ResetBits(GPIOx, Trig_Pin);
//    
//    DELAY_US(500);
//    //GPIO_ResetBits(GPIOx, Echo_Pin);
//    TIM3->CNT = 0;
//    TIM_Cmd(TIM3, ENABLE);
//}

void right_TRIG_Start(void)
{
	GPIO_SetBits(GPIOx,Trig_Pin);
    DELAY_US(10);
	GPIO_ResetBits(GPIOx,Trig_Pin);
    DELAY_MS(8);
}

float Measure_distance(void)
{
    printf("1\r\n");
	while (GPIO_ReadInputDataBit(GPIOx,Echo_Pin) == 1)
    {} //�ȵ�ECHOΪ�͵�ƽ�������ص�ֵ�ɹ��󣬲�������������
    
	printf("2\r\n");
    right_TRIG_Start();
   
	printf("3\r\n");
    while (GPIO_ReadInputDataBit(GPIOx,Echo_Pin) == 0)
    {}//�ȵ�ECHO���շ�������ʱ����ʱ��
        TIM_SetCounter(TIM3,0); //���������
        
    printf("4\r\n");
    TIM_Cmd(TIM3, ENABLE);  //ʹ�ܶ�ʱ��,��ʼ����
        
    printf("5\r\n");
    //uint16_t reture_value = 0;
    while (GPIO_ReadInputDataBit(GPIOx,Echo_Pin) == 1)
    {}//�ȵ�ECHOΪ�͵�ƽ��ֹͣ��ʱ
    
    TIM_Cmd(TIM3, DISABLE);	//ʧ�ܶ�ʱ��,��ֹ����	
        
	printf("6\r\n");
	return 	(TIM_GetCounter(TIM3))/1000000.0*340/2 *100;//��ʱ���ʱ��λcm
}

//void TIM3_IRQHandler(void)
//{
//    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
//    {
//        if (TIM3_Ultrasonic_FLAG1 == 1)
//        {
//            TIM3_Ultrasonic_FLAG1 = 0;              //�����־λ����
//            Ultrasonic_Time1 = Ultrasonic_Tim_old1; //���Ϊ����ֵ���þɵ�����
//        }
//        TIM_Cmd(TIM3, DISABLE); //�ض�ʱ����ʡ��һֱ���

//        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //����жϱ�־λ
//    }
//    
//    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) //���������¼�
//    {
//        TIM3_Ultrasonic_VAL = TIM_GetCapture1(TIM3);
//        
//        if (TIM3_Ultrasonic_FLAG1 == 1) //�Ѿ�����һ�������أ��ֲ�������½���
//        {
//            
//            TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Rising); //����Ϊ�����ز���Ϊ�´β���׼��

//            if (TIM3_Ultrasonic_VAL > 58 && TIM3_Ultrasonic_VAL < 29000) //�õ��ܵĸߵ�ƽ��ʱ��,����(1cm-500cm)*58us����Ĵ���ֵ,ʵ�ʷ�Χ(2cm-4500cm)
//            {
//                Ultrasonic_Time1 = TIM3_Ultrasonic_VAL;    //�����µ�����
//                Ultrasonic_Tim_old1 = TIM3_Ultrasonic_VAL; //���¾ɵ�����
//            }
//            else
//            {
//                Ultrasonic_Time1 = Ultrasonic_Tim_old1; //����ֵ���þɵ�����
//            }
//            
//            TIM3_Ultrasonic_FLAG1 = 0; //�����־λ����
//            TIM_Cmd(TIM3, DISABLE);    //�ض�ʱ����ʡ��һֱ���
//        }
//        else //��һ�β���������
//        {
//            TIM3->CNT = 0;                                       //��ն�ʱ������ֵ����֤�������ݶ�ʱ�������,�´β���ʱ�������Ǹߵ�ƽ����
//            TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Falling); //����Ϊ�½��ز��񣬵ȴ������½���
//            TIM3_Ultrasonic_FLAG1 = 1;                           //����Ѳ���������
//        }
//        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); //����жϱ�־λ
//    }
//}
