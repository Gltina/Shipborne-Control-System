#include "mpu6050.h"
#include "../I2C/i2c.h"

/**
  * @brief   д���ݵ�MPU6050�Ĵ���
  * @param   
  * @retval  
  */
void MPU6050_WriteReg(u8 reg_add, u8 reg_dat)
{
    I2C_ByteWrite(reg_dat, reg_add);
}

/**
  * @brief   ��MPU6050�Ĵ�����ȡ����
  * @param   
  * @retval  
  */
void MPU6050_ReadData(u8 reg_add, unsigned char *Read, u8 num)
{
    I2C_BufferRead(Read, reg_add, num);
}

/**
  * @brief   ��ʼ��MPU6050оƬ
  * @param   
  * @retval  
  */
void MPU6050_Init(void)
{
    MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x00);  //�������״̬
    MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV, 0x07);  //�����ǲ�����
    MPU6050_WriteReg(MPU6050_RA_CONFIG, 0x06);      //��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
    MPU6050_WriteReg(MPU6050_RA_CONFIG, 0x06);
    MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);  //�������Լ켰������Χ(2000deg/s)
    MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG, 0x01); //���ü��ٶȴ�����������2Gģʽ
}

/**
  * @brief   ��ȡMPU6050��ID
  * @param   
  * @retval  ��������1���쳣����0
  */
uint8_t MPU6050ReadID(void)
{
    unsigned char Re = 0;
    MPU6050_ReadData(MPU6050_RA_WHO_AM_I, &Re, 1); //��������ַ
    if (Re != 0x68)
    {
        #if MPU_DEBUG_ON
        MPU_ERROR("MPU6050 dectected error!\r\n");
        #endif
        return 1;
    }
    else
    {
        #if MPU_DEBUG_ON
        MPU_INFO("MPU6050 ID = %d\r\n", Re);
        #endif
        return 0;
    }
}

/**
  * @brief   ��ȡMPU6050�ļ��ٶ�����
  * @param   
  * @retval  
  */
void MPU6050ReadAcc(short *accData)
{
    u8 buf[6];
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   ��ȡMPU6050�ĽǼ��ٶ�����
  * @param   
  * @retval  
  */
void MPU6050ReadGyro(short *gyroData)
{
    u8 buf[6];
    MPU6050_ReadData(MPU6050_GYRO_OUT, buf, 6);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5]; 
}

/**
  * @brief   ��ȡMPU6050��ԭʼ�¶�����
  * @param   
  * @retval  
  */
void MPU6050ReadTemp(short *tempData)
{
    u8 buf[2];
    MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H, buf, 2); //��ȡ�¶�ֵ
    *tempData = (buf[0] << 8) | buf[1];
}

/**
  * @brief   ��ȡMPU6050���¶����ݣ�ת�������϶�
  * @param   
  * @retval  
  */
void MPU6050_ReturnTemp(float *Temperature)
{
    short temp3;
    u8 buf[2];

    MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H, buf, 2); //��ȡ�¶�ֵ
    temp3 = (buf[0] << 8) | buf[1];
    *Temperature = ((double)temp3 / 340.0) + 36.53;
}

void Read_MPU6050(short *accData, short *gyroData)
{
    // https://github.com/urbanzrim/stm32-MPU6050/blob/master/cmsis_lib/include/mpu6050.h
    
    MPU6050ReadAcc(accData);
    // ת��Ϊ�ɶ�����
    //accData[0] /= 16384.0;
    //accData[1] /= 16384.0;
    //accData[2] /= 16384.0;
    
    MPU6050ReadGyro(gyroData);
    
    /* @brief Get Gyroscope X,Y,Z calculated data
     *
     * @param X - sensor roll on X axis
     * @param Y - sensor pitch on Y axis
     * @param Z - sensor jaw on Z axis
     *
     * @retval @MPU6050_errorstatus
     */
    // ת��Ϊ�ɶ�����
    //gyroData[0] /= 131.0;
    //gyroData[1] /= 131.0;
    //gyroData[2] /= 131.0;
    
}