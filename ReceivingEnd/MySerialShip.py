import serial
import serial.tools.list_ports
import threading
import struct as s
from MsgEncode import *
import MsgCommon as MsgCom
import time

# 初始化编码器
msg_encode = MsgEncode(8)


class MySerialShip:
    # 记录接收的每条数据
    msg_list = []
    # 接收数据线程
    reading_threading_flag = True
    # 发送数据线程
    sending_threading_flag = True
    # 串口对象
    serial_object = serial.Serial()
    # 回应起点时间
    t_response = 0

    def __init__(self) -> None:
        pass

    def check_comports(self):
        port_list = list(serial.tools.list_ports.comports())
        print(port_list)

    def get_msg_size(self):
        return len(self.msg_list)

    def read_data(self, ser):
        # 循环接收数据，此为死循环，用线程实现
        newline_flag = 0
        curr_msg = bytearray()
        while self.reading_threading_flag:
            try:
                if ser.in_waiting:
                    rec_flag = s.unpack("c", ser.read(1))[0]

                    # 记录收到的\n数量, 满足两个则是报文起始位
                    if rec_flag == b"\n":
                        newline_flag += 1
                    else:
                        # 不是连续的话就重置
                        newline_flag = 0

                    # 当\n有两个即可开始接收数据, 实际是从缓冲区里取数据
                    if newline_flag == 2:
                        # 报文类型
                        data_type = s.unpack("H", ser.read(2))[0]

                        # 一般报文类型
                        if data_type == 0:
                            # 数据长度 H
                            total_byte_len = s.unpack("H", ser.read(2))[0]
                            # 接收剩下的数据, 后面的数据第一个是设备ID
                            # 前面的数据分别是: 标识头(2)+报文类型(2)+数据长度(2)
                            curr_msg.extend(ser.read(total_byte_len - 2 - 2 - 2))
                            # 添加到接收数据列表中
                            self.msg_list.append(curr_msg)
                            # 对数据进行解析
                            self.decode_msg(curr_msg, 0)

                            # 完成后, 标志已经收到数据
                            MsgCom.G_received_data_flag = True
                            # 清除当前报文, 准备接收下一条
                            curr_msg = bytearray()
                            newline_flag = 0

                            # 计算从发送回应到接收到新报文的耗时
                            # t_receive = time.perf_counter()
                            # print(
                            #     "[From PC] Consumed Time:{:.3f}s".format(
                            #         round(t_receive - self.t_response, 3)
                            #     )
                            # )

                        # 字符串类型
                        elif data_type == 1:
                            # 数据长度 H
                            total_byte_len = s.unpack("H", ser.read(2))[0]
                            # 接收剩下的数据, 后面的数据第一个是设备ID + 字符串
                            # 前面的数据分别是: 标识头(2)+报文类型(2)+数据长度(2)
                            curr_msg.extend(ser.read(total_byte_len - 2 - 2 - 2))
                            # 对数据进行解析
                            self.decode_msg(curr_msg, 1)

                            # 完成后, 标志已经收到数据,但不会作为标准数据
                            # MsgCom.G_received_data_flag = False
                            # 清除当前报文, 准备接收下一条
                            curr_msg = bytearray()
                            newline_flag = 0

            # end if
            except Exception as e:
                print("[Error] line:{}, {}".format(e.__traceback__.tb_lineno, e))
                continue
        # end while

        # 结束串口接收进程
        while self.serial_object.is_open():
            self.serial_object.close()

    def send_data(self, ser):
        while self.sending_threading_flag:
            if MsgCom.G_received_data_flag:
                try:
                    # time.sleep(1.0)
                    self.serial_object.write(msg_encode.make_encap_msg())
                    # self.t_response = time.perf_counter()
                    pass
                except Exception as e:
                    print("---error---: ", e)
                print("[From PC:] Response Sent ")
                MsgCom.G_received_data_flag = False

    def close_read_data_threading(self):
        self.reading_threading_flag = False

    def close_send_data_threading(self):
        self.sending_threading_flag = False

    def open_read_data_threading(self):
        # 判断是否打开成功
        if self.serial_object.is_open:
            self.reading_threading_flag = True
            r_t = threading.Thread(target=self.read_data, args=(self.serial_object,))
            r_t.setDaemon(True)
            r_t.start()
        else:
            self.serial_object.close()

    def open_send_data_threading(self):
        if self.serial_object.is_open:
            self.sending_threading_flag = True
            s_t = threading.Thread(target=self.send_data, args=(self.serial_object,))
            s_t.setDaemon(True)
            s_t.start()
        else:
            self.serial_object.close()

    def open_port(self, portx, bps, timeout):
        try:
            # 打开串口，并得到串口对象
            self.serial_object = serial.Serial(portx, bps, timeout=timeout)
            # ser.set_buffer_size() 默认4096 够大了
        except Exception as e:
            print("---error---: ", e)

    def decode_msg(self, msg, msg_type):
        if msg_type == 0:
            res = s.unpack("<HhhhhhhfdfffBBBBBBBB", msg)
            print(
                "[From Slave:]设备ID:{}, 陀螺仪数值:A:{},{},{} G:{},{},{}, 温度:{}".format(
                    res[0],
                    res[1],
                    res[2],
                    res[3],
                    res[4],
                    res[5],
                    res[6],
                    res[7],
                    res[8],
                )
            )
        elif msg_type == 1:
            device_id = s.unpack("H", msg[:2])[0]
            print("[From Slave] ID:{}:{}".format(device_id, msg[2:].decode("utf-8")))

    # return self.serial_object
