#include "deviceManage.h"

//extern struct device_data_s device_data;

uint8_t DS18B20ID[8];
device_data_s device_data;
device_status_s device_status;

void init_device_status()
{
    memset(&device_data, 0, sizeof(device_data_s));
    memset(&device_status, 0, sizeof(device_status_s));

    /*
    memset(&device_data.Accelerate, '0', sizeof(short)*3);
    memset(&device_data.Gyroscope, '0', sizeof(short)*3);
    device_data.WaterDepth = 0;
    device_data.Temperature = 0;
    device_data.WaterSpeed = 0;
    device_data.WaterTankDepth = 0;
    */
}

int8_t Init_Device()
{
    Delay_Init(72);

    // USART
    USART_Configuration();

    /* LED 端口初始化 */
    LED_Init();

    // I2C
    I2C_Bus_Init();
    
    //主电机初始化
    ENGINE775_Init();
    
    // DS18B20
    while (DS18B20_Init())
    {
        printf("\r\n no ds18b20 exit \r\n");
    }
    DS18B20_ReadId(DS18B20ID);

    // MPU6050
    MPU6050_Init();
    MPU6050ReadID();

    // 超声波传感器
    //SRF05_Init();

    // 舵机
    MG90S_Init();
    
    // 水压传感器
    WaterPress_Init();
    // 水箱控制模块
    WaterTank_Init();
    // 初始化水箱液面和水压传感器的ADC
    Init_ADC1();
    Init_DMA();
    
    // 水流计速器
    //S201C_Init();
    
    // 启动自动发送模块
    SendingEnd_Init();

    return 0;
}

void read_device_data()
{
    // TODO:检查全部状态
    //int8_t status;

    // 温度传感器
    device_data.Temperature =
        DS18B20_GetTemp_MatchRom(DS18B20ID);

    // 陀螺仪/加速度传感器
    Read_MPU6050(device_data.Accelerate, device_data.Gyroscope);
    //MPU6050ReadAcc(device_data.Accelerate);
    //MPU6050ReadGyro(device_data.Gyroscope);
    //MPU6050_ReturnTemp(&device_data.Temp);

    // 超声波传感器
    //device_data.distance = Measure_distance();

    // 获取水压传感器和水箱液面传感器的数值
    Get_ADC1_Value(&device_data);
    
}

void report_device_data()
{
    //printf("\r\nT:%.2f C\r\n", device_data.Temperature);

    //printf("%d %d %d\r\n", device_data.Accelerate[0], device_data.Accelerate[1], device_data.Accelerate[2]);
    //printf("%d %d %d\r\n", device_data.Gyroscope[0], device_data.Gyroscope[1], device_data.Gyroscope[2]);
    //printf("%f\r\n", device_data.Temp);

    //printf("D:%fcm\r\n", device_data.distance);

    //printf("W:%.2fcm\r\n", device_data.water_depth);

    //printf("S:%.2fL/min\r\n", device_data.water_speed);

    //clear_device_data();

    //test_send();

    // 输出44
    //printf("%d\r\n", sizeof(device_data));
    //printf("%d\r\n", sizeof(device_data.device_status));

    //encap_msg_sending();
}

void clear_device_data()
{
    //device_data.WaterSpeed = 0.0;
}

void test_send()
{
    Usart_SendString(USARTx, "sending\r\n");

    /*
    (10+36) * 2 + 1 = 72+1=73 这是一行的字节数
    总共146字节, 估计1s多一点不到两秒可以完成
    一秒大概119字节
    
    */
    char *test_send_data =
        "\
0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\r\n\
0 1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u v w x y z\r\n";

    Usart_SendString(USARTx, test_send_data);

    Usart_SendString(USARTx, "over\r\n");

    //DELAY_MS(2000);
    //printf("%d\r\n", strlen(test_send_data));
}

//static int32_t send_index = 0;

void EncapMsgSending(uint8_t data_type, char *send_str)
{
    // 设置发送报文头
    sending_header_s sh;
    sh.header[0] = '\n';
    sh.header[1] = '\n'; // 设置报文头
    sh.device_ID = 99;   // 设置潜艇编号

    // 设置发送报文包
    sending_package_s sp;
    sp.header_p = &sh;

    // 数据类型为报文, 即包括了传感器数据和设备状态
    if (data_type == 0)
    {
        sh.data_type = 0; // 报文类型为一般报文类型
        sh.data_length =  // 设置发送数据长度(包括传感器数据和设备状态数据)
            sizeof(sending_header_s) + sizeof(device_data_s) + sizeof(device_status_s);

        // 设置发送内容地址
        sending_content_s sc;
        sc.device_data_p = &device_data;
        sc.device_status_p = &device_status;

        sp.content_p = &sc;

        // 发送报文头+传感器数值+设备状态
        // 可能发的太快了, 可以加一个延时
        Usart_SendByLength(USARTx, (char *)sp.header_p, sizeof(sending_header_s));
        DELAY_MS(50);
        Usart_SendByLength(USARTx, (char *)sp.content_p->device_data_p, sizeof(device_data_s));
        DELAY_MS(50);
        Usart_SendByLength(USARTx, (char *)sp.content_p->device_status_p, sizeof(device_status_s));
    }
    // 字符串数据
    else if (data_type == 1)
    {
        if (send_str == NULL)
            return;

        sh.data_type = 1;
        sh.data_length = sizeof(sending_header_s) + strlen(send_str);

        // 设置发送报文包, 但是内容不设置,仅使用包头
        Usart_SendByLength(USARTx, (char *)sp.header_p, sizeof(sending_header_s));
        Usart_SendString(USARTx, send_str);
    }

    //Usart_SendByte(USARTx, sp.tail);

    //    send_index++;
    //    if(send_index == 3)
    //    {
    //        while(1){}
    //    }
}

void ApplyControlSignal(device_status_s *new_status)
{

    //    printf("%d %d %d %d %d %d %d %d #", new_status->MotorGear,
    //           new_status->Rudder0Angle,
    //           new_status->Rudder1Angle,
    //           new_status->Highbeam,
    //           new_status->Taillight,
    //           new_status->Headlight,
    //           new_status->WaterIn,
    //           new_status->WaterOut
    //    );

    if (device_status.MotorGear != new_status->MotorGear)
    {
        device_status.MotorGear = new_status->MotorGear;
        // action
    }
    if (device_status.Rudder0Angle != new_status->Rudder0Angle)
    {
        device_status.Rudder0Angle = new_status->Rudder0Angle;
        // 测试舵机
        if (device_status.Rudder0Angle == 0)
        {
            Servo_Control(0);
        }
        else if (device_status.Rudder0Angle == 1)
        {
            Servo_Control(180);
        }
    }
    if (device_status.Rudder1Angle != new_status->Rudder1Angle)
    {
        device_status.Rudder1Angle = new_status->Rudder1Angle;
        // action
    }
    if (new_status->Highbeam == 0 || new_status->Highbeam == 1)
    {
        device_status.Highbeam = new_status->Highbeam;
        
        if(device_status.Highbeam == 1)
        {
            Open_SignalLED();
        }
        else if(device_status.Highbeam == 0)
        {
            Close_SignalLED();
        }
    }
    if (device_status.Taillight != new_status->Taillight)
    {
        device_status.Taillight = new_status->Taillight;
        // action
    }
    if (device_status.Headlight != new_status->Headlight)
    {
        device_status.Headlight = new_status->Headlight;
        // action
    }
    // 设置进水口状态
    if (new_status->WaterIn == 0 || new_status->WaterIn == 1)
    {
        device_status.WaterIn = new_status->WaterIn;

        set_waterTank_IN_status(device_status.WaterIn);

        //encap_msg_sending(1,"set in");
    }
    // 设置出水口状态
    if (new_status->WaterOut == 0 || new_status->WaterOut == 1)
    {
        device_status.WaterOut = new_status->WaterOut;

        set_waterTank_OUT_status(device_status.WaterOut);

        //encap_msg_sending(1,"set out");
    }
}

void Init_ADC1()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    //ADC_DeInit(ADC1);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC时钟 一般不超过14MHZ 否则精度不准确  72MHZ/6=12MHZ
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  //ADC 工作模式:独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                        //AD 通道模式;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                  //AD 转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      //ADC 数据右对齐;
    ADC_InitStructure.ADC_NbrOfChannel = ADC_M;                //顺序进行规则转换的 ADC 
    ADC_Init(ADC1, &ADC_InitStructure);
   
    // 4个通道的采集顺序
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5); //设定ADC规则组 // 水深1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_239Cycles5); //设定ADC规则组 // 水深1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_239Cycles5); //设定ADC规则组 // 液面1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_239Cycles5); //设定ADC规则组 // 液面2
    
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE); //使能ADC1

    // ADC 开始校准
    ADC_ResetCalibration(ADC1);
    //等待复位校准结束
    while (ADC_GetResetCalibrationStatus(ADC1));
    //重置指定的 ADC 的校准寄存器
    ADC_StartCalibration(ADC1);
    //等待校准结束
    while (ADC_GetCalibrationStatus(ADC1));
    
    //由于没有采用外部触发，所以使用软件触发 ADC 转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void Init_DMA()
{
    DMA_InitTypeDef DMA_InitStructure;
    
    //DMA_DeInit(DMA1_Channel1);
    
    /*开启DMA时钟*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    
    
    /*设置DMA外设ADC地址*/
    DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t)&( ADC1->DR );

    /*设置DMA内存基地址*/
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_value;
    
    /*数据源来自外设*/        
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;    

    /*传输大小*/    
    DMA_InitStructure.DMA_BufferSize = ADC_N * ADC_M;

    /*外设寄存器只有一个，地址不用递增*/      
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

    /*存储器地址递增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    

    /*ReceiveBuff数据单位*/    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

    /*SENDBUFF_SIZE数据单位*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;     

    /*DMA模式：循环模式*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular ;

    /*优先级：中, 当使用一个ADC时, 优先级无影响*/    
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  

    /*使能内存到内存的传输    */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    /*配置DMA1的1通道, ADC1 对应 DMA1 通道 1，ADC3 对应 DMA2 通道 5，ADC2 没有 DMA 功能*/           
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);        
    
    /*失能DMA1的1通道，一旦使能就开始传输*/
    DMA_Cmd (DMA1_Channel1,ENABLE);
}

void filter_ADC1_value()
{
    //#define ADC_N 10    // 每个通道采集十次 
    //#define ADC_M 4     // 四个通道
    //static uint16_t AD_value[ADC_N][ADC_M]; // AD采集的结果
    //static float AD_filtered_value[ADC_M];  // AD过滤后的值
     
    for(uint8_t i =0;i< ADC_M; i++)
    {
        uint32_t total_value = 0;
        for(uint8_t j =0;j< ADC_N; j++)
        {
//            char value[2];
//            sprintf(value, "%d", AD_value[j][i]);
//            EncapMsgSending(1,value);
            
            total_value += AD_value[j][i];
        }
        AD_filtered_value[i] = ((float)total_value / ADC_N)* 3.3 / 4096.0;
    }
}

void Get_ADC1_Value(device_data_s * device_data)
{    
    // 等待转换完成
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
    DMA_ClearFlag(DMA1_FLAG_TC1);
    
    filter_ADC1_value();
    
    if(ADC_M < 4) return;
    
    device_data->WaterDepth0    = AD_filtered_value[0];
    device_data->WaterDepth1    = AD_filtered_value[1];
    device_data->WaterTankDepth0= AD_filtered_value[2];
    device_data->WaterTankDepth1= AD_filtered_value[3];
    
}