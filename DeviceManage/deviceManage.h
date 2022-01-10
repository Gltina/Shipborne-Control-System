#ifndef _DEVICEMANAGE_H_
#define _DEVICEMANAGE_H_

#include <limits.h>
#include <string.h>

#include "../stm32f10x_it.h"

#include "../delay/delay.h"

#include "../I2C/i2c.h"
#include "../USART/usart.h"
#include "../WaterTank/waterTank.h"
#include "../WaterPress/waterPress.h"
#include "../MPU6050/mpu6050.h"
#include "../DS18B20/ds18b20.h"
#include "../S201C/s201c.h"
#include "../MG90S/mg90s.h"
#include "../SRF05/srf05.h"
#include "../LED/led.h"

//extern int16_t ADC1_VALUE;

// �����ڴ�������⽨��:
// https://www.geeksforgeeks.org/is-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member/

// �豸״̬
typedef struct device_status_s
{
    // �����λ
    uint8_t MotorGear;
    
    // β���0�Ƕ�
    uint8_t Rudder0Angle;
    
    // β���1�Ƕ�
    uint8_t Rudder1Angle;
    
    // LED������
    uint8_t Highbeam;
    
    // LEDβ��
    uint8_t Taillight;
    
    // LEDͷ��
    uint8_t Headlight;
    
    // ��ˮ��״̬
    uint8_t WaterIn;
    
    // ��ˮ��״̬
    uint8_t WaterOut;
    
} device_status_s;

// ��������ֵ
typedef struct device_data_s
{
    // ������/������������, 2 byte, �ܹ�2*6=12�ֽ�
    short Accelerate[3];
    short Gyroscope[3];
    //float Temp;

    // �¶ȴ�����, ��C, 4 byte
    float Temperature;
    uint8_t DS18B20ID[8];

    // ������������, 4 byte
    //float distance;

    // ˮ����ֵ (cm) , 4 byte
    float WaterDepth;

    // ˮ���ٶ� (L/min), 4 byte
    float WaterSpeed;
    
    // ˮ��Һ����� (cm) , 4 byte
    float WaterTankDepth;
}device_data_s;

typedef struct sending_header_s
{
    // ���ı�ͷ
    uint8_t header[2];
    
    // �������ݳ���
    uint16_t data_length;
    
    // �������豸��ΨһID
    uint16_t device_ID;
    
}sending_header_s;

typedef struct sending_content_s
{
    // �豸״̬
    device_status_s * device_status_p;
    // �豸����
    device_data_s * device_data_p;
}sending_content_s;

// ���ͱ��ĸ�ʽ
typedef struct sending_package_s
{
    // һ��Ϊ'\n'
    sending_header_s * header_p;
    
    // ��������
    sending_content_s * content_p;
    
    // һ��Ϊ#
    //uint8_t tail;
}sending_package_s;

int8_t init_device();

void init_device_status();

void read_device_data();

void report_device_data();

void clear_device_data();

void test_send();

void encap_msg_sending();

#endif