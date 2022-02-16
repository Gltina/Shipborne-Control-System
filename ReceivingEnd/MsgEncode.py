import struct as s


class MsgEncode:

    # 报文头标识符, 实际为2个
    data_header = "\n"  # uuint_8
    # 控制数据长度
    control_data_length = 0  # uint16_t
    # 主机编号
    principal_computer_ID = 123  # uint16_t
    # 控制信号码,
    control_list = []
    # 待发送数据
    send_data = bytearray()

    # 初始化时需要设置控制位的数量
    def __init__(self, control_signal_num) -> None:
        self.control_list = [int(0)] * control_signal_num
        # 电机档位 uint8_t MotorGear
        self.control_list[0] = ord('s')
        # LED警示灯 uint8_t Cautionlight
        self.control_list[4] = 1

        pass

    # 生成可发送的字节流
    def make_encap_msg(self):
        """
        self.send_data = bytearray(
            [
                ord(self.data_header),
                self.principal_computer_ID,
                self.control_data_length,
                *self.control_list,
            ]
        )"""
        # 2个header
        header_fmt = "<BBHH"
        control_fmt = "B" * len(self.control_list)
        # 设置发送数据长度
        self.control_data_length = s.calcsize(header_fmt + control_fmt)
        # 生成字节包
        self.send_data = s.pack(
            header_fmt + control_fmt,
            # 报文标识头
            ord(self.data_header),
            ord(self.data_header),
            # 报文数据长度
            self.control_data_length,
            # 报文发送设备的ID, 也就是本机ID
            self.principal_computer_ID,
            # 报文控制信号
            *self.control_list
        )
        # print(self.send_data)
        return self.send_data

    def get_encap_msg(self):
        self.make_encap_msg()
        return self.send_data

    def change_control_signal(self, key, value):
        if key == "MotorGear":
            self.control_list[0] = value
        elif key == "Rudder0Angle":
            self.control_list[1] = value
        elif key == "Rudder1Angle":
            self.control_list[2] = value
        elif key == "Highbeam":
            self.control_list[3] = value
        elif key == "Cautionlight":
            self.control_list[4] = value
        elif key == "WaterIn":
            self.control_list[5] = value
        elif key == "WaterOut":
            self.control_list[6] = value
        elif key == "SystemStatus0":
            self.control_list[7] = value
        elif key == "SystemStatus1":
            self.control_list[8] = value
        elif key == "SystemStatus2":
            self.control_list[9] = value

        # print("[From PC--]", *self.control_list)
