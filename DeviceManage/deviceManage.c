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

    /* LED �˿ڳ�ʼ�� */
    LED_Init();

    // I2C
    I2C_Bus_Init();
    
    //�������ʼ��
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

    // ������������
    //SRF05_Init();

    // ���
    MG90S_Init();
    
    // ˮѹ������
    WaterPress_Init();
    // ˮ�����ģ��
    WaterTank_Init();
    // ��ʼ��ˮ��Һ���ˮѹ��������ADC
    Init_ADC1();
    Init_DMA();
    
    // ˮ��������
    //S201C_Init();
    
    // �����Զ�����ģ��
    SendingEnd_Init();

    return 0;
}

void read_device_data()
{
    // TODO:���ȫ��״̬
    //int8_t status;

    // �¶ȴ�����
    device_data.Temperature =
        DS18B20_GetTemp_MatchRom(DS18B20ID);

    // ������/���ٶȴ�����
    Read_MPU6050(device_data.Accelerate, device_data.Gyroscope);
    //MPU6050ReadAcc(device_data.Accelerate);
    //MPU6050ReadGyro(device_data.Gyroscope);
    //MPU6050_ReturnTemp(&device_data.Temp);

    // ������������
    //device_data.distance = Measure_distance();

    // ��ȡˮѹ��������ˮ��Һ�洫��������ֵ
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

    // ���44
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
    (10+36) * 2 + 1 = 72+1=73 ����һ�е��ֽ���
    �ܹ�146�ֽ�, ����1s��һ�㲻������������
    һ����119�ֽ�
    
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
    // ���÷��ͱ���ͷ
    sending_header_s sh;
    sh.header[0] = '\n';
    sh.header[1] = '\n'; // ���ñ���ͷ
    sh.device_ID = 99;   // ����Ǳͧ���

    // ���÷��ͱ��İ�
    sending_package_s sp;
    sp.header_p = &sh;

    // ��������Ϊ����, �������˴��������ݺ��豸״̬
    if (data_type == 0)
    {
        sh.data_type = 0; // ��������Ϊһ�㱨������
        sh.data_length =  // ���÷������ݳ���(�������������ݺ��豸״̬����)
            sizeof(sending_header_s) + sizeof(device_data_s) + sizeof(device_status_s);

        // ���÷������ݵ�ַ
        sending_content_s sc;
        sc.device_data_p = &device_data;
        sc.device_status_p = &device_status;

        sp.content_p = &sc;

        // ���ͱ���ͷ+��������ֵ+�豸״̬
        // ���ܷ���̫����, ���Լ�һ����ʱ
        Usart_SendByLength(USARTx, (char *)sp.header_p, sizeof(sending_header_s));
        DELAY_MS(50);
        Usart_SendByLength(USARTx, (char *)sp.content_p->device_data_p, sizeof(device_data_s));
        DELAY_MS(50);
        Usart_SendByLength(USARTx, (char *)sp.content_p->device_status_p, sizeof(device_status_s));
    }
    // �ַ�������
    else if (data_type == 1)
    {
        if (send_str == NULL)
            return;

        sh.data_type = 1;
        sh.data_length = sizeof(sending_header_s) + strlen(send_str);

        // ���÷��ͱ��İ�, �������ݲ�����,��ʹ�ð�ͷ
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
        // ���Զ��
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
    // ���ý�ˮ��״̬
    if (new_status->WaterIn == 0 || new_status->WaterIn == 1)
    {
        device_status.WaterIn = new_status->WaterIn;

        set_waterTank_IN_status(device_status.WaterIn);

        //encap_msg_sending(1,"set in");
    }
    // ���ó�ˮ��״̬
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
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); //����ADCʱ�� һ�㲻����14MHZ ���򾫶Ȳ�׼ȷ  72MHZ/6=12MHZ
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  //ADC ����ģʽ:����ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                        //AD ͨ��ģʽ;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                  //AD ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ת��������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      //ADC �����Ҷ���;
    ADC_InitStructure.ADC_NbrOfChannel = ADC_M;                //˳����й���ת���� ADC 
    ADC_Init(ADC1, &ADC_InitStructure);
   
    // 4��ͨ���Ĳɼ�˳��
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5); //�趨ADC������ // ˮ��1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_239Cycles5); //�趨ADC������ // ˮ��1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_239Cycles5); //�趨ADC������ // Һ��1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_239Cycles5); //�趨ADC������ // Һ��2
    
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE); //ʹ��ADC1

    // ADC ��ʼУ׼
    ADC_ResetCalibration(ADC1);
    //�ȴ���λУ׼����
    while (ADC_GetResetCalibrationStatus(ADC1));
    //����ָ���� ADC ��У׼�Ĵ���
    ADC_StartCalibration(ADC1);
    //�ȴ�У׼����
    while (ADC_GetCalibrationStatus(ADC1));
    
    //����û�в����ⲿ����������ʹ��������� ADC ת��
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void Init_DMA()
{
    DMA_InitTypeDef DMA_InitStructure;
    
    //DMA_DeInit(DMA1_Channel1);
    
    /*����DMAʱ��*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    
    
    /*����DMA����ADC��ַ*/
    DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t)&( ADC1->DR );

    /*����DMA�ڴ����ַ*/
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_value;
    
    /*����Դ��������*/        
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;    

    /*�����С*/    
    DMA_InitStructure.DMA_BufferSize = ADC_N * ADC_M;

    /*����Ĵ���ֻ��һ������ַ���õ���*/      
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

    /*�洢����ַ����*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    

    /*ReceiveBuff���ݵ�λ*/    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

    /*SENDBUFF_SIZE���ݵ�λ*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;     

    /*DMAģʽ��ѭ��ģʽ*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular ;

    /*���ȼ�����, ��ʹ��һ��ADCʱ, ���ȼ���Ӱ��*/    
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  

    /*ʹ���ڴ浽�ڴ�Ĵ���    */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    /*����DMA1��1ͨ��, ADC1 ��Ӧ DMA1 ͨ�� 1��ADC3 ��Ӧ DMA2 ͨ�� 5��ADC2 û�� DMA ����*/           
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);        
    
    /*ʧ��DMA1��1ͨ����һ��ʹ�ܾͿ�ʼ����*/
    DMA_Cmd (DMA1_Channel1,ENABLE);
}

void filter_ADC1_value()
{
    //#define ADC_N 10    // ÿ��ͨ���ɼ�ʮ�� 
    //#define ADC_M 4     // �ĸ�ͨ��
    //static uint16_t AD_value[ADC_N][ADC_M]; // AD�ɼ��Ľ��
    //static float AD_filtered_value[ADC_M];  // AD���˺��ֵ
     
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
    // �ȴ�ת�����
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
    DMA_ClearFlag(DMA1_FLAG_TC1);
    
    filter_ADC1_value();
    
    if(ADC_M < 4) return;
    
    device_data->WaterDepth0    = AD_filtered_value[0];
    device_data->WaterDepth1    = AD_filtered_value[1];
    device_data->WaterTankDepth0= AD_filtered_value[2];
    device_data->WaterTankDepth1= AD_filtered_value[3];
    
}