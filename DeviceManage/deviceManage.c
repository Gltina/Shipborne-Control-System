#include "deviceManage.h"

struct device_data_s device_data;
uint8_t ucDs18b20Id[8];


//void init_device_status(int *device, int size)
//{
//    for (int i = 0; i < size; i++)
//        device[i] = 0;
//}

int8_t init_device()
{
    delay_init(72);
    
     // USART
    USART_Configuration();
    
    /* LED 端口初始化 */
    LED_GPIO_Config();

    // I2C
    //I2C_Bus_Init();

    // DS18B20
    //while (DS18B20_Init())
    //{
    //    printf("\r\n no ds18b20 exit \r\n");
    //}
    //DS18B20_ReadId(ucDs18b20Id);

    // MPU6050
    //MPU6050_Init();
    //MPU6050ReadID();
    
    // 超声波传感器
    //SRF05_Init();
    
    // 舵机
    //MG90S_Init();
    
    // 水压传感器
    //WaterPress_Init();
    
    // 水流计速器
    S201C_Init();
    
    return 0;
}

void read_device_data()
{
    // TODO:检查全部状态
    //int8_t status;
    
    // 温度传感器
    //device_data.temperature =
    //    DS18B20_GetTemp_MatchRom(ucDs18b20Id);

    // 陀螺仪/加速度传感器
    //MPU6050ReadAcc(device_data.Acel);
    //MPU6050ReadGyro(device_data.Gyro);
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
    //printf("\r\nT:%.2f C\r\n", device_data.temperature);
    
    //printf("%d %d %d\r\n", device_data.Acel[0], device_data.Acel[1], device_data.Acel[2]);
    //printf("%d %d %d\r\n", device_data.Gyro[0], device_data.Gyro[1], device_data.Gyro[2]);
    //printf("%f\r\n", device_data.Temp);
    
    //printf("D:%fcm\r\n", device_data.distance);
    
    //printf("W:%.2fcm\r\n", device_data.water_depth);
    
    //printf("S:%.2fL/min\r\n", device_data.water_speed);
    
    //clear_device_data();
    
    test_send();
    //printf("%c", 'A');
}

void clear_device_data()
{
    device_data.water_speed = 0.0;
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
