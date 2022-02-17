#include "deviceManage.h"

//extern struct device_data_s device_data;

uint8_t DS18B20ID[8];

void init_device_status_data(device_data_s *device_data, device_status_s *device_status)
{
    memset(device_data, 0, sizeof(device_data_s));
    memset(device_status, 0, sizeof(device_status_s));

    // 注意: 需要和上位机同步

    // 主动力电机停机
    device_status->MotorGear = 's';

    // 打开警示灯
    device_status->Cautionlight = 1;

    // 检查接受缓冲区大小是否合格
    if (RECESIZE != sizeof(receiving_package_s))
    {
        LED_WORKING_ROOT = 0;
    }
}

int8_t init_device()
{
    Delay_Init(72);

    // USART
    USART_Configuration();

    /* LED 端口初始化 */
    LED_Init();

    // I2C
    //I2C_Bus_Init();

    //主电机初始化
    Engine775_Init();

    // DS18B20
    //while (DS18B20_Init())
    //{
    //    EncapTextMsgSending("No ds18b20 exit \r\n");
    //}
    //DS18B20_ReadId(DS18B20ID);

    // MPU6050
    //MPU6050_Init();
    //MPU6050ReadID();

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

void read_device_data(device_data_s *device_data_p)
{
    // TODO:检查全部状态
    //int8_t status;

    // 温度传感器
    //device_data_p->Temperature =
    //   DS18B20_GetTemp_MatchRom(DS18B20ID);

    // 陀螺仪/加速度传感器
    //Read_MPU6050(device_data_p->Accelerate, device_data_p->Gyroscope);

    // 超声波传感器
    //device_data.distance = Measure_distance();

    // 获取水压传感器和水箱液面传感器的数值
    Get_ADC1_Value(device_data_p);
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

void EncapMsgSending(uint8_t data_type, device_data_s *device_data_p, device_status_s *device_status_p, const char *send_str)
{
    // 设置发送报文头
    sending_header_s sh;
    sh.header[0] = '\n';
    sh.header[1] = '\n'; // 设置报文头
    sh.device_ID = 99;   // 设置潜艇编号

    // 数据类型为报文, 即包括了传感器数据和设备状态
    if (data_type == 0)
    {
        sh.data_type = 0; // 报文类型为一般报文类型
        sh.data_length =  // 设置发送数据长度(包括传感器数据和设备状态数据)
            sizeof(sending_header_s) + sizeof(device_data_s) + sizeof(device_status_s);

        // 发送报文头+传感器数值+设备状态
        // 可能发的太快了, 可以加一个延时
        Usart_SendByLength(USART1, (char *)&sh, sizeof(sending_header_s));
        DELAY_MS(50);
        Usart_SendByLength(USART1, (char *)device_data_p, sizeof(device_data_s));
        DELAY_MS(50);
        Usart_SendByLength(USART1, (char *)device_status_p, sizeof(device_status_s));
    }

    // 字符串数据, 忽略一般格式的报文内容, 仅使用字符串内容
    else if (data_type == 1)
    {
        if (send_str == NULL)
            return;

        sh.data_type = 1;
        sh.data_length = sizeof(sending_header_s) + strlen(send_str);

        // 设置发送报文包, 但是内容不设置,仅使用包头
        Usart_SendByLength(USART1, (char *)&sh, sizeof(sending_header_s));
        Usart_SendString(USART1, send_str);
    }
}

void apply_control_commmand(device_status_s *curr_status, device_status_s *new_status)
{
    // 调整主动力电机档位
    if (curr_status->MotorGear != new_status->MotorGear)
    {
        Engine_Control(new_status->MotorGear);

        curr_status->MotorGear = get_curr_engine_status();
    }
    DELAY_MS(50);
    
    // 调整舵机角度
    if (curr_status->Rudder0Angle != new_status->Rudder0Angle)
    {
        Rudder0_set_angle(new_status->Rudder0Angle);

        curr_status->Rudder0Angle = new_status->Rudder0Angle;
    DELAY_MS(200);
    }
    if (curr_status->Rudder1Angle != new_status->Rudder1Angle)
    {
        Rudder1_set_angle(new_status->Rudder1Angle);

        curr_status->Rudder1Angle = new_status->Rudder1Angle;
    DELAY_MS(200);
    }

    // 调整远关灯
    if (curr_status->Highbeam != new_status->Highbeam)
    {
        if (new_status->Highbeam == 1)
        {
            LED_HIGHBEAM_ON;
        }
        else if (new_status->Highbeam == 0)
        {
            LED_HIGHBEAM_OFF;
        }
        curr_status->Highbeam = new_status->Highbeam;
    }
    // 调整警示灯
    if (curr_status->Cautionlight != new_status->Cautionlight)
    {
        curr_status->Cautionlight = new_status->Cautionlight;

        LED_CAUTION_ROOT = curr_status->Cautionlight;
    }
    // 设置进水口状态
    if (curr_status->WaterIn == 0 || curr_status->WaterIn == 1)
    {
        curr_status->WaterIn = new_status->WaterIn;

        WaterTank_set_IN_status(curr_status->WaterIn);
        DELAY_MS(50);
    }
    
    // 设置出水口状态
    if (curr_status->WaterOut == 0 || curr_status->WaterOut == 1)
    {
        curr_status->WaterOut = new_status->WaterOut;

        WaterTank_set_OUT_status(curr_status->WaterOut);
      
    DELAY_MS(50);
    }
    // 设置系统状态
    if (curr_status->SystemStatus0 != new_status->SystemStatus0)
    {
        curr_status->SystemStatus0 = new_status->SystemStatus0;
    }

    if (curr_status->SystemStatus1 != new_status->SystemStatus1)
    {
        curr_status->SystemStatus1 = new_status->SystemStatus1;
    }
}

void Init_ADC1()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    //ADC_DeInit(ADC1);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); //设置ADC时钟 一般不超过14MHZ 否则精度不准确  72MHZ/6=12MHZ
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  //ADC 工作模式:独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                        //AD 通道模式;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                  //AD 转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //ADC 数据右对齐;
    ADC_InitStructure.ADC_NbrOfChannel = ADC_M;                         //顺序进行规则转换的 ADC
    ADC_Init(ADC1, &ADC_InitStructure);

    // 4个通道的采集顺序
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5); //设定ADC规则组 // 水深1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_239Cycles5); //设定ADC规则组 // 水深1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_239Cycles5); //设定ADC规则组 // 液面1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_239Cycles5); //设定ADC规则组 // 液面2

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE); //使能ADC1

    // 校准ADC
    ADC1_Calibration();

    //由于没有采用外部触发，所以使用软件触发 ADC 转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void ADC1_Calibration()
{
    // ADC 开始校准
    ADC_ResetCalibration(ADC1);
    //等待复位校准结束
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;
    //重置指定的 ADC 的校准寄存器
    ADC_StartCalibration(ADC1);
    //等待校准结束
    while (ADC_GetCalibrationStatus(ADC1))
        ;
}

void Init_DMA()
{
    DMA_InitTypeDef DMA_InitStructure;

    //DMA_DeInit(DMA1_Channel1);

    /*开启DMA时钟*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /*设置DMA外设ADC地址*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->DR);

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
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

    /*优先级：中, 当使用一个ADC时, 优先级无影响*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;

    /*使能内存到内存的传输    */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    /*配置DMA1的1通道, ADC1 对应 DMA1 通道 1，ADC3 对应 DMA2 通道 5，ADC2 没有 DMA 功能*/
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /*失能DMA1的1通道，一旦使能就开始传输*/
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

void Get_ADC1_Value(device_data_s *device_data)
{
    // 等待转换完成
    while (!DMA_GetFlagStatus(DMA1_FLAG_TC1))
        ;
    float AD_filtered_value[ADC_M]; // AD过滤后的值
    filter_ADC1_value(AD_filtered_value);

    DMA_ClearFlag(DMA1_FLAG_TC1);

    if (ADC_M < 4)
        return;

    device_data->WaterDepth0 = WaterDepth_Func(AD_filtered_value[0] * 3.3 / 4096);
    device_data->WaterDepth1 = WaterDepth_Func(AD_filtered_value[1] * 3.3 / 4096);

    device_data->WaterTankDepth0 = WaterTankLevel_Func(AD_filtered_value[2] * 3.3 / 4096);
    device_data->WaterTankDepth1 = WaterTankLevel_Func(AD_filtered_value[3] * 3.3 / 4096);
}

void filter_ADC1_value(float *AD_filtered_value)
{
    for (uint8_t i = 0; i < ADC_M; ++i)
    {
        uint16_t value_tmp[ADC_N] = {};

        for (uint8_t j = 0; j < ADC_N; ++j)
        {
            value_tmp[j] = AD_value[j][i];
        }

        AD_filtered_value[i] =
            MVA_Filtering(value_tmp, ADC_N);
    }
}

float WaterDepth_Func(float voltage)
{
    return voltage; //(voltage*426 - 411.415);
}

float WaterTankLevel_Func(float voltage)
{
    return voltage;
}

void EncapTextMsgSending(const char * text)
{    
    EncapMsgSending(1, NULL, NULL, text);
}