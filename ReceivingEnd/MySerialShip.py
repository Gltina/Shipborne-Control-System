import serial
from serial.serialutil import Timeout
import serial.tools.list_ports
import threading
import struct as s

received_data_completed = []


class MySerialShip:
    def __init__(self) -> None:
        pass

    def check_comports(self):
        port_list = list(serial.tools.list_ports.comports())
        print(port_list)

    def read_data(self, ser):
        # 循环接收数据，此为死循环，可用线程实现
        xs = bytearray()
        received_flag = False
        received_byte_len = 0
        total_byte_len = 99999
        while True:
            if ser.in_waiting:
                rec_data_tmp = ser.read(1)
                # 取出第一个判断(其实也只有一个有效的)
                if s.unpack("c", rec_data_tmp)[0] is b"\n":
                    # 设备编号
                    rec_data_tmp = ser.read(2)
                    xs.extend(rec_data_tmp)
                    # 数据长度
                    rec_data_tmp = ser.read(2)
                    xs.extend(rec_data_tmp)
                    total_byte_len = s.unpack("H", rec_data_tmp)[0]
                    # 接收剩下的数据
                    rec_data_tmp = ser.read(total_byte_len - 2 - 2)
                    xs.extend(rec_data_tmp)

                    # 解析全部数据 实际使用不该放在这里
                    received_data_completed.append(xs)
                    res = s.unpack("<HHhhhhhhfdfffBBBBBBBB", xs)
                    print("设备编号:{}, 数据长度:{}".format(res[0], res[1]))
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
                    xs = bytearray()

    def open_port(self, portx, bps, timeout):
        ret = False
        try:
            # 打开串口，并得到串口对象
            ser = serial.Serial(portx, bps, timeout=timeout)
            # ser.set_buffer_size()
            # 判断是否打开成功
            if ser.is_open:
                ret = True
                threading.Thread(target=self.read_data, args=(ser,)).start()
            else:
                ser.close()
        except Exception as e:
            print("---error---: ", e)
        return ser, ret
