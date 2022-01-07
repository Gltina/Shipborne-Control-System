#ifndef _DEVICEMANAGE_H_
#define _DEVICEMANAGE_H_
#include "string.h"

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
extern struct device_data_s device_data;

struct device_data_s
{
    // 陀螺仪/加速器传感器, 2 byte
    short Acel[3];
    short Gyro[3];
    //float Temp;

    // 温度传感器, °C, 4 byte
    float temperature;

    // 超声波传感器, 4 byte
    float distance;

    // 水深数值 (cm) , 4 byte
    float water_depth;

    // 水流速度 (L/min), 4 byte
    float water_speed;
};

//void init_device_status(int *device, int size);

int8_t init_device();

void read_device_data();

void report_device_data();

void clear_device_data();

void test_send();

#endif