
#include "DeviceManage/deviceManage.h"
#include "stm32f10x_it.h"

//#define SHOW_TIME1

void process_sending(device_data_s *device_data_p, device_status_s *device_status_p);

uint8_t process_receiving(device_status_s *received_device_status);

int main(void)
{
    // 初始化传感器
    init_device();

    // 初始化潜艇状态和传感器数据
    device_data_s device_data; device_status_s device_status;
    init_device_status_data(&device_data, &device_status);

    while (1)
    {
        // 处理接收数据
        device_status_s received_device_status;
        if (process_receiving(&received_device_status) == 0)
        {
            // 应用接收数据
            apply_control_commmand(&device_status, &received_device_status);
        }

        // 处理发送数据
        process_sending(&device_data, &device_status);

        LED_period_control(1, &LED_PERIOD[0]);
        LED_period_control(3, &LED_PERIOD[1]);
    }

    return 0;
}

void process_sending(device_data_s *device_data_p, device_status_s *device_status_p)
{
    // 已发送判断是否超时
    if (REPORT_DATA_STATUS == 1)
    {
        // 等待超时
        if ((REPORT_TIM_CAPTURE * 200) > WAITINGSECOND) // 将$(REPORT_TIM_CAPTURE)转为毫秒
        {
            ALARM_COUNT++;
            ALARM_COUNT_TEST++;

            // 达到报警次数
            if (ALARM_COUNT == ALARM_MAX_COUNT)
            {
                // TODO
                // 紧急情况
                LED_WORKING_ROOT = 0;
                LED_CAUTION_ROOT = 1;
                LED_CAUTION_ON;
                while (1)
                {
                }
            }
            else // 尝试重新发送
            {
                //LED_CAUTION_ON;
                REPORT_DATA_STATUS = 0; // 标记未发送, 再次尝试发送
            }
        }
        else
        {
            // 未超时, 在等待回应
        }
    }
    else if (REPORT_DATA_STATUS == 0)
    {
        // 发送一般报文
        read_device_data(device_data_p);
        EncapMsgSending(0, device_data_p, device_status_p, NULL);

        TIM2->CNT = 0;
        REPORT_DATA_STATUS = 1; // 标记数据已发送
        REPORT_TIM_CAPTURE = 0; // 计数器清零
    }
}

uint8_t process_receiving(device_status_s *received_device_status)
{
    if (RECEIVE_COMPLETED == 1)
    {
        // 此时上位机数据接受完成
        // 接受的串口缓冲区应该为空
        receiving_package_s received_package;
        // 转换到结构体
        memcpy(&received_package, REC_DATA, sizeof(REC_DATA));
        // 获取上位机发送的状态
        memcpy(received_device_status, &received_package.device_status, sizeof(device_status_s));

#ifdef SHOW_TIME1
        // 计算从发送报文到回应完全接收的耗时
        char send_str[70];
        uint32_t receive_delay = TIM2->CNT + REPORT_TIM_CAPTURE * 2000;
        sprintf(send_str, "ID:%d,Length:%d,['%c',%d,%d,%d,%d,%d,%d,%d,%d,%d] T1=%.3fs (alarm count:%d)",
                received_package.device_ID, received_package.data_length,
                received_package.device_status.MotorGear, received_package.device_status.Rudder0Angle,
                received_package.device_status.Rudder1Angle, received_package.device_status.Highbeam,
                received_package.device_status.Cautionlight, received_package.device_status.WaterIn,
                received_package.device_status.WaterOut, received_package.device_status.SystemStatus0,
                received_package.device_status.SystemStatus1, received_package.device_status.SystemStatus2,
                receive_delay / 10000.0, ALARM_COUNT_TEST);
        EncapMsgSending(1, NULL, NULL, send_str);
        memset(send_str, '\0', strlen(send_str));
#endif //SHOW_TIME1

        // 清理工作
        memset((void *)REC_DATA, '\0', RECESIZE); // 清空接收区域
        //memset((void *)&received_package, '\0', sizeof(receiving_package_s)); // 清空实际接收报文

        REC_NUM = 2;            // 重置接收数量
        ALARM_COUNT = 0;        // 取消警报次数
        NEWLINE_FLAG = 0;       // 重置接收报文标志头
        RECEIVE_COMPLETED = 0;  // 重新标志未完全接收
        REPORT_DATA_STATUS = 0; // 状态改为继续发送报告, 放置在最后变化
        LED_WORKING_ROOT = 1;   // 重启工作状态指示灯

        return 0;
    }
    return 1;
}
