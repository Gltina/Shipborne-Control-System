import struct as s


class MsgEncode:

    # 报文头标识符
    data_header = "\n"
    # 控制数据长度
    control_data_length = 0
    # 主机编号
    principal_computer_ID = 123
    # 控制信号码,
    control_list = []
    # 待发送数据
    send_data = bytearray()

    # 初始化时需要设置控制位的数量
    def __init__(self, control_signal_num) -> None:
        self.control_list = [int(0)] * control_signal_num
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
        header_fmt = "BHH"
        control_fmt = "B" * len(self.control_list)
        self.send_data = s.pack(
            header_fmt + control_fmt,
            ord(self.data_header),
            self.principal_computer_ID,
            self.control_data_length,
            *self.control_list
        )
        # print(self.send_data)
        return self.send_data

    def get_encap_msg(self):
        self.make_encap_msg()
        return self.send_data
