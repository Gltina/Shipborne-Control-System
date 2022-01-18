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
//#include "../S201C/s201c.h"
#include "../SendingEnd/sendingEnd.h"
#include "../Engine755/engine775.h"
#include "../MG90S/mg90s.h"
#include "../SRF05/srf05.h"
#include "../LED/led.h"

//extern int16_t ADC1_VALUE;

// �����ڴ�������⽨��:
// https://www.geeksforgeeks.org/is-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member/

#define SHOW_TIME1

// ADC
#define ADC_N 10    // ÿ��ͨ���ɼ�ʮ�� 
#define ADC_M 4     // �ĸ�ͨ��
static uint16_t AD_value[ADC_N][ADC_M]; // AD�ɼ��Ľ��

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
    // ϵͳ״̬����ֵ
    uint8_t SystemStatus0;
    // ϵͳ״̬����ֵ
    uint8_t SystemStatus1;
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
    //uint8_t DS18B20ID[8];
    // ������������, 4 byte
    //float distance;
    // ˮ����ֵ (cm) , 4 byte
    float WaterDepth0;
    float WaterDepth1;
    
    // ˮ���ٶ� (L/min), 4 byte
    //float WaterSpeed;
    // ˮ��Һ����� (cm) , 4 byte
    float WaterTankDepth0;
    float WaterTankDepth1;
} device_data_s;

typedef struct sending_header_s
{
    // ���ı�ͷ
    uint8_t header[2];
    // ��������
    uint16_t data_type;
    // �������ݳ���
    uint16_t data_length;
    // �������豸��ΨһID
    uint16_t device_ID;
} sending_header_s;

typedef struct sending_content_s
{
    // �豸״̬
    device_status_s *device_status_p;
    // �豸����
    device_data_s *device_data_p;
} sending_content_s;

// ���ͱ��ĸ�ʽ
typedef struct sending_package_s
{
    // ���ͱ���ͷ
    sending_header_s *header_p;
    // ��������
    sending_content_s *content_p;
} sending_package_s;

// ���ձ��ĸ�ʽ
typedef struct receiving_package_s
{
    // ���ı�ͷ, Ĭ������'\n'
    uint8_t header[2];
    // �������ݳ���
    uint16_t data_length;
    // ��λ���豸��ID
    uint16_t device_ID;
    // �豸״̬, �Դ���Ϊ�����ź�
    device_status_s device_status;
} receiving_package_s;

int8_t Init_Device();

void init_device_status();

void read_device_data();

void report_device_data();

void clear_device_data();

void test_send();

/*
    ��ʼ��ADC, Ϊˮѹ��ˮ��Һ�洫������׼��
*/
void Init_ADC1();

/*
    ��ʼ��DMA
*/
void Init_DMA();

/*
    ��ȡADC1����ֵ
    Ҳ����ˮѹ��������ˮ��Һ�洫��������ֵ
*/
void Get_ADC1_Value(device_data_s * device_data);

/*
    У׼ADC1
    
*/
void ADC1_Calibration();

// ��ADC������˵�ָ������
//
void filter_ADC1_value(float *AD_filtered_value);

/*
    ��װ���ҷ���,֧�ַ���һ�㱨�ĺ��ַ�������
    @data_type: 0 ���� 1
    
    ��@data_typeΪ1ʱ��Ҫ�ṩ@send_str
    ��Ҫ��ϸ���send_str�ĺϷ�����(��Ҫ\0��β)
*/
void EncapMsgSending(uint8_t data_type, char *send_str);

/*
    ����@new_status�޸�����status
    һ����˵,@new_status������λ�����͵���
*/
void ApplyControlSignal(device_status_s *new_status);

float WaterDepth_Func(float voltage);

float WaterTankLevel_Func(float voltage);

#endif