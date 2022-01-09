#include "deviceManage.h"

//extern struct device_data_s device_data;

device_data_s device_data;
device_status_s device_status;

void init_device_status()
{
    memset(&device_data, 0, sizeof(device_data_s));
    
    /*
    memset(&device_data.Accelerate, '0', sizeof(short)*3);
    memset(&device_data.Gyroscope, '0', sizeof(short)*3);
    device_data.WaterDepth = 0;
    device_data.Temperature = 0;
    device_data.WaterSpeed = 0;
    device_data.WaterTankDepth = 0;
    */
}

int8_t init_device()
{
    delay_init(72);
    
     // USART
    USART_Configuration();
    
    /* LED 端口初始化 */
    LED_GPIO_Config();

    // I2C
    I2C_Bus_Init();

    // DS18B20
    while (DS18B20_Init())
    {
        printf("\r\n no ds18b20 exit \r\n");
    }
    DS18B20_ReadId(device_data.DS18B20ID);

    // MPU6050
    MPU6050_Init();
    MPU6050ReadID();
    
    // 超声波传感器
    //SRF05_Init();
    
    // 舵机
    //MG90S_Init();
    
    // 水压传感器
    //WaterPress_Init();
    
    // 水流计速器
    //S201C_Init();
    
    // 水箱控制模块
    //WaterTank_Init();
    
    return 0;
}

void read_device_data()
{
    // TODO:检查全部状态
    //int8_t status;
    
    // 温度传感器
    device_data.Temperature =
        DS18B20_GetTemp_MatchRom(device_data.DS18B20ID);

    // 陀螺仪/加速度传感器
    MPU6050ReadAcc(device_data.Accelerate);
    MPU6050ReadGyro(device_data.Gyroscope);
    //MPU6050_ReturnTemp(&device_data.Temp);
    
    // 超声波传感器
    //device_data.distance = Measure_distance();
    
    //  水压传感器
    // 自动定时获取 adc会有一个采集频率
    //device_data.water_depth = ADC2WaterDepth(ADC1_VALUE);
    //device_data.water_depth = Get_WaterPress_Filtered();
    
    // 水流数值
    // 自动采集
}

void report_device_data()
{
    printf("\r\nT:%.2f C\r\n", device_data.Temperature);
    
    printf("%d %d %d\r\n", device_data.Accelerate[0], device_data.Accelerate[1], device_data.Accelerate[2]);
    printf("%d %d %d\r\n", device_data.Gyroscope[0], device_data.Gyroscope[1], device_data.Gyroscope[2]);
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
    device_data.WaterSpeed = 0.0;
}

void test_send()
{
    Usart_SendString(USARTx, "sending\r\n");
    
    /*
    (10+36) * 2 + 1 = 72+1=73 这是一行的字节数
    总共146字节, 估计1s多一点不到两秒可以完成
    一秒大概119字节
    
    */
    char * test_send_data = 
        "\
0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\r\n\
0 1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u v w x y z\r\n";
    
    Usart_SendString(USARTx, test_send_data);
    
    Usart_SendString(USARTx, "over\r\n");
    
    //DELAY_MS(2000);
    //printf("%d\r\n", strlen(test_send_data));
}

static int32_t send_index = 0;

void encap_msg_sending()
{
    // 设置潜艇编号
    device_data.device_ID = 99;
    // 设置发送数据长度(包括传感器数据和设备状态数据)
    device_data.data_length =
        sizeof(device_data_s) + sizeof(device_status_s);
    
    // 准备发送数据
    sending_content_s sc;
    sc.device_data_p = &device_data;
    sc.device_status_p = &device_status;
    
    sending_package_s sp;
    
    sp.header = '\n';
    sp.content_p = &sc;
    //sp.tail = '\n';
    
    // 发送
    Usart_SendByte(USARTx, sp.header);
    Usart_SendByLength(USARTx,(char *) sp.content_p->device_data_p, sizeof(device_data_s));
    Usart_SendByLength(USARTx,(char *) sp.content_p->device_status_p, sizeof(device_status_s));
    //Usart_SendByte(USARTx, sp.tail);
    
//    send_index++;
//    if(send_index == 3)
//    {
//        while(1){}
//    }
}