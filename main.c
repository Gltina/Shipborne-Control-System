
#include "DeviceManage/deviceManage.h"
#include "stm32f10x_it.h"

//#define SHOW_TIME1

void process_sending(device_data_s *device_data_p, device_status_s *device_status_p);

uint8_t process_receiving(device_status_s *received_device_status);

int main(void)
{
    // ��ʼ��������
    init_device();

    // ��ʼ��Ǳͧ״̬�ʹ���������
    device_data_s device_data; device_status_s device_status;
    init_device_status_data(&device_data, &device_status);

    while (1)
    {
        // �����������
        device_status_s received_device_status;
        if (process_receiving(&received_device_status) == 0)
        {
            // Ӧ�ý�������
            apply_control_commmand(&device_status, &received_device_status);
        }

        // ����������
        process_sending(&device_data, &device_status);

        LED_period_control(1, &LED_PERIOD[0]);
        LED_period_control(3, &LED_PERIOD[1]);
    }

    return 0;
}

void process_sending(device_data_s *device_data_p, device_status_s *device_status_p)
{
    // �ѷ����ж��Ƿ�ʱ
    if (REPORT_DATA_STATUS == 1)
    {
        // �ȴ���ʱ
        if ((REPORT_TIM_CAPTURE * 200) > WAITINGSECOND) // ��$(REPORT_TIM_CAPTURE)תΪ����
        {
            ALARM_COUNT++;
            ALARM_COUNT_TEST++;

            // �ﵽ��������
            if (ALARM_COUNT == ALARM_MAX_COUNT)
            {
                // TODO
                // �������
                LED_WORKING_ROOT = 0;
                LED_CAUTION_ROOT = 1;
                LED_CAUTION_ON;
                while (1)
                {
                }
            }
            else // �������·���
            {
                //LED_CAUTION_ON;
                REPORT_DATA_STATUS = 0; // ���δ����, �ٴγ��Է���
            }
        }
        else
        {
            // δ��ʱ, �ڵȴ���Ӧ
        }
    }
    else if (REPORT_DATA_STATUS == 0)
    {
        // ����һ�㱨��
        read_device_data(device_data_p);
        EncapMsgSending(0, device_data_p, device_status_p, NULL);

        TIM2->CNT = 0;
        REPORT_DATA_STATUS = 1; // ��������ѷ���
        REPORT_TIM_CAPTURE = 0; // ����������
    }
}

uint8_t process_receiving(device_status_s *received_device_status)
{
    if (RECEIVE_COMPLETED == 1)
    {
        // ��ʱ��λ�����ݽ������
        // ���ܵĴ��ڻ�����Ӧ��Ϊ��
        receiving_package_s received_package;
        // ת�����ṹ��
        memcpy(&received_package, REC_DATA, sizeof(REC_DATA));
        // ��ȡ��λ�����͵�״̬
        memcpy(received_device_status, &received_package.device_status, sizeof(device_status_s));

#ifdef SHOW_TIME1
        // ����ӷ��ͱ��ĵ���Ӧ��ȫ���յĺ�ʱ
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

        // ������
        memset((void *)REC_DATA, '\0', RECESIZE); // ��ս�������
        //memset((void *)&received_package, '\0', sizeof(receiving_package_s)); // ���ʵ�ʽ��ձ���

        REC_NUM = 2;            // ���ý�������
        ALARM_COUNT = 0;        // ȡ����������
        NEWLINE_FLAG = 0;       // ���ý��ձ��ı�־ͷ
        RECEIVE_COMPLETED = 0;  // ���±�־δ��ȫ����
        REPORT_DATA_STATUS = 0; // ״̬��Ϊ�������ͱ���, ���������仯
        LED_WORKING_ROOT = 1;   // ��������״ָ̬ʾ��

        return 0;
    }
    return 1;
}
