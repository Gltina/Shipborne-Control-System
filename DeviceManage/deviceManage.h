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

// 关于内存对其的理解建议:
// https://www.geeksforgeeks.org/is-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member/

#define SHOW_TIME1

// ADC
#define ADC_N 10    // 每个通道采集十次 
#define ADC_M 4     // 四个通道
static uint16_t AD_value[ADC_N][ADC_M]; // AD采集的结果

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
    // 系统状态控制值
    uint8_t SystemStatus0;
    // 系统状态控制值
    uint8_t SystemStatus1;
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
    //uint8_t DS18B20ID[8];
    // 超声波传感器, 4 byte
    //float distance;
    // 水深数值 (cm) , 4 byte
    float WaterDepth0;
    float WaterDepth1;
    
    // 水流速度 (L/min), 4 byte
    //float WaterSpeed;
    // 水箱液面深度 (cm) , 4 byte
    float WaterTankDepth0;
    float WaterTankDepth1;
} device_data_s;

typedef struct sending_header_s
{
    // 报文表头
    uint8_t header[2];
    // 数据类型
    uint16_t data_type;
    // 发送数据长度
    uint16_t data_length;
    // 本发射设备的唯一ID
    uint16_t device_ID;
} sending_header_s;

typedef struct sending_content_s
{
    // 设备状态
    device_status_s *device_status_p;
    // 设备读数
    device_data_s *device_data_p;
} sending_content_s;

// 发送报文格式
typedef struct sending_package_s
{
    // 发送报文头
    sending_header_s *header_p;
    // 发送内容
    sending_content_s *content_p;
} sending_package_s;

// 接收报文格式
typedef struct receiving_package_s
{
    // 报文表头, 默认两个'\n'
    uint8_t header[2];
    // 发送数据长度
    uint16_t data_length;
    // 上位机设备的ID
    uint16_t device_ID;
    // 设备状态, 以此作为控制信号
    device_status_s device_status;
} receiving_package_s;

int8_t Init_Device();

void init_device_status();

void read_device_data();

void report_device_data();

void clear_device_data();

void test_send();

/*
    初始化ADC, 为水压和水箱液面传感器做准备
*/
void Init_ADC1();

/*
    初始化DMA
*/
void Init_DMA();

/*
    获取ADC1的数值
    也就是水压传感器和水箱液面传感器的数值
*/
void Get_ADC1_Value(device_data_s * device_data);

/*
    校准ADC1
    
*/
void ADC1_Calibration();

// 将ADC数组过滤到指定数组
//
void filter_ADC1_value(float *AD_filtered_value);

/*
    包装并且发送,支持发送一般报文和字符串数据
    @data_type: 0 或者 1
    
    当@data_type为1时需要提供@send_str
    需要仔细检查send_str的合法长度(需要\0结尾)
*/
void EncapMsgSending(uint8_t data_type, char *send_str);

/*
    根据@new_status修改现有status
    一般来说,@new_status是由上位机发送得来
*/
void ApplyControlSignal(device_status_s *new_status);

float WaterDepth_Func(float voltage);

float WaterTankLevel_Func(float voltage);

#endif