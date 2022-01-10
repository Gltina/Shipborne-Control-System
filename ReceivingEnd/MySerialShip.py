from io import TextIOBase
import serial
import serial.tools.list_ports
import threading
import struct as s
from MsgEncode import *
import MsgCommon as MsgCom


class MySerialShip:
    # 记录接收的每条数据
    msg_list = []
    # 接收数据线程
    reading_threading_flag = True
    # 发送数据线程
    sending_threading_flag = True
    # 串口对象
    serial_object = serial.Serial()

    # 初始化编码器
    msg_encode = MsgEncode(8)

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
        other_msg = bytearray()
        while self.reading_threading_flag:
            if ser.in_waiting:
                # TODO : 处理单个, 根据累计来判断
                rec_data_tmp = ser.read(1)
                # 取出第一个判断(其实也只有一个有效的)
                try:
                    rec_flag = s.unpack("c", rec_data_tmp)[0]
                except Exception as e:
                    print("---error---: ", e)
                    continue

                # 记录收到的\n数量, 满足两个则是报文起始位
                if rec_flag == b"\n":
                    newline_flag += 1

                if newline_flag == 2:
                    newline_flag = 0
                    try:
                        # 数据长度
                        rec_data_tmp = ser.read(2)
                        total_byte_len = s.unpack("H", rec_data_tmp)[0]
                        # print("数据长度:{}".format(total_byte_len))
                        curr_msg.extend(rec_data_tmp)
                        # 接收剩下的数据
                        rec_data_tmp = ser.read(total_byte_len - 2)
                        curr_msg.extend(rec_data_tmp)

                        # 添加到接收数据列表中
                        self.msg_list.append(curr_msg)

                        # 解析全部数据 实际使用不该放在这里
                        res = s.unpack("<HHhhhhhhfdfffBBBBBBBB", curr_msg)
                        print("数据长度:{}, 设备编号:{}".format(res[0], res[1]))
                        print(
                            "陀螺仪数值:A:{},{},{} G:{},{},{}, 温度:{}".format(
                                res[2],
                                res[3],
                                res[4],
                                res[5],
                                res[6],
                                res[7],
                                res[8],
                            )
                        )
                        MsgCom.G_received_data_flag = True
                        # print(self.get_msg_size())
                        # 清除当前报文, 准备接收下一条
                        curr_msg = bytearray()
                    except Exception as e:
                        print("---error---: ", e)
                        continue

                else:
                    if b"#" == rec_flag:
                        print(other_msg.decode("utf-8"))
                        other_msg = bytearray()
                    else:
                        other_msg.extend(rec_data_tmp)

        # 结束串口接收进程
        try:
            while self.serial_object.is_open():
                self.serial_object.close()
        except Exception as e:
            print("---error---: ", e)

    def send_data(self, ser):
        while self.sending_threading_flag:
            if MsgCom.G_received_data_flag:
                try:
                    self.msg_encode.make_encap_msg()
                    self.serial_object.write(self.msg_encode.get_encap_msg())
                except Exception as e:
                    print("---error---: ", e)
                print("response sent")
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

    # return self.serial_object
