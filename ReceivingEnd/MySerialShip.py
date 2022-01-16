import serial
import serial.tools.list_ports
import threading
import struct as s
from MsgEncode import *
from MsgDecode import *
import MsgCommon as MsgCom
import time
from multiprocessing import Semaphore

# 初始化编码器
msg_encode = MsgEncode(10)
msg_decode = MsgDecode()

# 显示发送回应报文到完全接收潜艇报文的耗时
# SHOW_TIME2 = True
# 初始化信号量
se = Semaphore(1)


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
    t_total = 0
    # 通讯次数
    communication_count = 0

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
                    # print(rec_flag)

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
                            curr_msg.extend(
                                ser.read(total_byte_len - 2 - 2 - 2))
                            # 添加到接收数据列表中
                            self.msg_list.append(curr_msg)
                            # 对数据进行解析
                            msg_decode.decode_msg(curr_msg, 0)

                            # 显示从发送回应报文到接收一般报文的耗时
                            self.communication_count += 1
                            t_receive = time.perf_counter()
                            t_diff = t_receive - self.t_response
                            self.t_total += t_diff
                            print(
                                "[From PC] #{} Consumed Time(T2):{:.3f}s".format(
                                    self.communication_count,
                                    round(self.t_total /
                                          self.communication_count, 3)
                                )
                            )

                            # 发送回应报文
                            self.serial_object.write(
                                msg_encode.make_encap_msg())
                            print("[From PC] Response Sent ")
                            self.t_response = time.perf_counter()

                        # 字符串类型
                        elif data_type == 1:
                            # 数据长度 H
                            total_byte_len = s.unpack("H", ser.read(2))[0]
                            # 接收剩下的数据, 后面的数据第一个是设备ID + 字符串
                            # 前面的数据分别是: 标识头(2)+报文类型(2)+数据长度(2)
                            curr_msg.extend(
                                ser.read(total_byte_len - 2 - 2 - 2))
                            # 对数据进行解析
                            msg_decode.decode_msg(curr_msg, 1)

                            # 完成后, 标志已经收到数据,但不会作为标准数据
                            # MsgCom.G_received_data_flag = False
                            # 清除当前报文, 准备接收下一条
                        else:
                            print("[From PC] Not a type that can detecte.")

                        curr_msg = bytearray()
                        newline_flag = 0
                    else:
                        #  if newline_flag != 2:
                        pass
            # end if ser.in_waiting
            except Exception as e:
                print("[Error] line:{}, {}".format(
                    e.__traceback__.tb_lineno, e))
                curr_msg = bytearray()
                newline_flag = 0
                continue
        # end while
        # 结束串口接收进程
        while self.serial_object.is_open():
            self.serial_object.close()

    # def send_data(self, ser):
    #     while self.sending_threading_flag:
    #         if MsgCom.G_received_data_flag:
    #             try:
    #                 # time.sleep(1.0)
    #                 self.serial_object.write(msg_encode.make_encap_msg())
    #                 if SHOW_TIME2:
    #                     self.t_response = time.perf_counter()
    #                 pass
    #             except Exception as e:
    #                 print("---error---: ", e)
    #             print("[From PC] Response Sent ")
    #             MsgCom.G_received_data_flag = False

    def close_read_data_threading(self):
        self.reading_threading_flag = False

    # def close_send_data_threading(self):
    #     self.sending_threading_flag = False

    def open_read_data_threading(self):
        # 判断是否打开成功
        if self.serial_object.is_open:
            self.reading_threading_flag = True
            r_t = threading.Thread(target=self.read_data,
                                   args=(self.serial_object,))
            r_t.setDaemon(True)
            r_t.start()
        else:
            self.serial_object.close()

    # def open_send_data_threading(self):
    #     if self.serial_object.is_open:
    #         self.sending_threading_flag = True
    #         s_t = threading.Thread(target=self.send_data,
    #                                args=(self.serial_object,))
    #         s_t.setDaemon(True)
    #         s_t.start()
    #     else:
    #         self.serial_object.close()

    def open_port(self, portx, bps, timeout):
        try:
            # 打开串口，并得到串口对象
            self.serial_object = serial.Serial(portx, bps, timeout=timeout)
            self.serial_object.set_buffer_size(
                rx_size=4096, tx_size=4096)  # 默认4096 够大了
        except Exception as e:
            print("---error---: ", e)

    # return self.serial_object
