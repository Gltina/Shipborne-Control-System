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
    
    /* LED �˿ڳ�ʼ�� */
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
    
    // ������������
    //SRF05_Init();
    
    // ���
    //MG90S_Init();
    
    // ˮѹ������
    //WaterPress_Init();
    
    // ˮ��������
    S201C_Init();
    
    return 0;
}

void read_device_data()
{
    // TODO:���ȫ��״̬
    //int8_t status;
    
    // �¶ȴ�����
    //device_data.temperature =
    //    DS18B20_GetTemp_MatchRom(ucDs18b20Id);

    // ������/���ٶȴ�����
    //MPU6050ReadAcc(device_data.Acel);
    //MPU6050ReadGyro(device_data.Gyro);
    //MPU6050_ReturnTemp(&device_data.Temp);
    
    // ������������
    //device_data.distance = Measure_distance();
    
    //  ˮѹ������
    // �Զ���ʱ��ȡ adc����һ���ɼ�Ƶ��
    //device_data.water_depth = ADC2WaterDepth(ADC1_VALUE);
    //device_data.water_depth = Get_WaterPress_Filtered();
    
    // ˮ����ֵ
    // �Զ��ɼ�
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
    (10+36) * 2 + 1 = 72+1=73 ����һ�е��ֽ���
    �ܹ�146�ֽ�, ����1s��һ�㲻������������
    һ����119�ֽ�
    
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
