#include "srf05.h"

//以此方案一个定时器可以简单扩展4通道采集
//实际范围(2cm-450cm),抛弃(1cm-500cm)*58us以外的错误值，就是正常值
//捕获上升沿清空定时器，正确数据就不会溢出500*58=29000<65536
//假如溢出证明雷达无反应,此时极限最大过去65536*1us=65ms 大约正好为推荐周期60ms
//也就是65ms以上扫描雷达，前后两次不会干涉,正好用溢出中断处理雷达超时,雷达有没有反应都处理到位了，保证准确性
//异常值推定物体未移动，采用上次有效值

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

//Tout（溢出时间）=（ARR+1)(PSC+1)/Tclk
//计数频率=Tclk/(PSC+1)=72m/72=1m
void TIM3_SRF05_Init(u16 arr, u16 psc)
{
    //printf("xxxx=%d\r\n",arr);
    //GPIO_InitTypeDef GPIO_InitStructure;
    
    //TIM_ICInitTypeDef TIM_ICInitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;

    //TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); //设置缺省值,这一步最好加上防止放到串口初始化后出问题
    //TIM_OCStructInit(&TIM_OCInitStructure);        //设置缺省值,这一步最好加上防止放到串口初始化后出问题
    //TIM_ICStructInit(&TIM_ICInitStructure); //设置缺省值,这一步最好加上防止放到串口初始化后出问题

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
    {} //等到ECHO为低电平，即返回的值成功后，才启动超声波。
    
	printf("2\r\n");
    right_TRIG_Start();
   
	printf("3\r\n");
    while (GPIO_ReadInputDataBit(GPIOx,Echo_Pin) == 0)
    {}//等到ECHO接收返回数据时，计时。
        TIM_SetCounter(TIM3,0); //清零计数器
        
    printf("4\r\n");
    TIM_Cmd(TIM3, ENABLE);  //使能定时器,开始计数
        
    printf("5\r\n");
    //uint16_t reture_value = 0;
    while (GPIO_ReadInputDataBit(GPIOx,Echo_Pin) == 1)
    {}//等到ECHO为低电平才停止计时
    
    TIM_Cmd(TIM3, DISABLE);	//失能定时器,截止计数	
        
	printf("6\r\n");
	return 	(TIM_GetCounter(TIM3))/1000000.0*340/2 *100;//用时间计时单位cm
}

//void TIM3_IRQHandler(void)
//{
//    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
//    {
//        if (TIM3_Ultrasonic_FLAG1 == 1)
//        {
//            TIM3_Ultrasonic_FLAG1 = 0;              //捕获标志位清零
//            Ultrasonic_Time1 = Ultrasonic_Tim_old1; //溢出为错误值，用旧的数据
//        }
//        TIM_Cmd(TIM3, DISABLE); //关定时器，省的一直溢出

//        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除中断标志位
//    }
//    
//    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) //发生捕获事件
//    {
//        TIM3_Ultrasonic_VAL = TIM_GetCapture1(TIM3);
//        
//        if (TIM3_Ultrasonic_FLAG1 == 1) //已经捕获到一个上升沿，又捕获的是下降沿
//        {
//            
//            TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Rising); //设置为上升沿捕获，为下次捕获准备

//            if (TIM3_Ultrasonic_VAL > 58 && TIM3_Ultrasonic_VAL < 29000) //得到总的高电平的时间,抛弃(1cm-500cm)*58us以外的错误值,实际范围(2cm-4500cm)
//            {
//                Ultrasonic_Time1 = TIM3_Ultrasonic_VAL;    //更新新的数据
//                Ultrasonic_Tim_old1 = TIM3_Ultrasonic_VAL; //更新旧的数据
//            }
//            else
//            {
//                Ultrasonic_Time1 = Ultrasonic_Tim_old1; //错误值，用旧的数据
//            }
//            
//            TIM3_Ultrasonic_FLAG1 = 0; //捕获标志位清零
//            TIM_Cmd(TIM3, DISABLE);    //关定时器，省的一直溢出
//        }
//        else //第一次捕获到上升沿
//        {
//            TIM3->CNT = 0;                                       //清空定时器计数值，保证正常数据定时器不溢出,下次捕获时间正好是高电平长度
//            TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Falling); //设置为下降沿捕获，等待捕获下降沿
//            TIM3_Ultrasonic_FLAG1 = 1;                           //标记已捕获到上升沿
//        }
//        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); //清除中断标志位
//    }
//}
