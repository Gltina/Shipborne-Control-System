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

// 关于内存对其的理解建议:
// https://www.geeksforgeeks.org/is-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member/

// 设备状态
typedef struct device_status_s
{
    // 电机档位
    uint8_t MotorGear;
    
    // 尾翼舵0角度
    uint8_t Rudder0Angle;
    
    // 尾翼舵1角度
    uint8_t Rudder1Angle;
    
    // LED照明灯
    uint8_t Highbeam;
    
    // LED尾灯
    uint8_t Taillight;
    
    // LED头灯
    uint8_t Headlight;
    
    // 进水口状态
    uint8_t WaterIn;
    
    // 排水口状态
    uint8_t WaterOut;
    
} device_status_s;

// 传感器数值
typedef struct device_data_s
{
    // 陀螺仪/加速器传感器, 2 byte, 总共2*6=12字节
    short Accelerate[3];
    short Gyroscope[3];
    //float Temp;

    // 温度传感器, °C, 4 byte
    float Temperature;
    uint8_t DS18B20ID[8];

    // 超声波传感器, 4 byte
    //float distance;

    // 水深数值 (cm) , 4 byte
    float WaterDepth;

    // 水流速度 (L/min), 4 byte
    float WaterSpeed;
    
    // 水箱液面深度 (cm) , 4 byte
    float WaterTankDepth;
}device_data_s;

typedef struct sending_header_s
{
    // 报文表头
    uint8_t header[2];
    
    // 发送数据长度
    uint16_t data_length;
    
    // 本发射设备的唯一ID
    uint16_t device_ID;
    
}sending_header_s;

typedef struct sending_content_s
{
    // 设备状态
    device_status_s * device_status_p;
    // 设备读数
    device_data_s * device_data_p;
}sending_content_s;

// 发送报文格式
typedef struct sending_package_s
{
    // 一般为'\n'
    sending_header_s * header_p;
    
    // 发送内容
    sending_content_s * content_p;
    
    // 一般为#
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