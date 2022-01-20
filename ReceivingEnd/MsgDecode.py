import struct as s

# please check https://docs.python.org/3/library/struct.html to know more details on this module.
# sample: https://www.journaldev.com/17401/python-struct-pack-unpack


class MsgDecode:
    # 一般报文的解析格式
    fmt_str = '<HhhhhhhfffffBBBBBBBBBB'

    def __init__(self) -> None:
        self.original_msg = ""
        self.decoded_msg = ""

    def read_from_file(self, filename):
        with open(filename, mode="rb") as f:
            while True:
                self.original_msg = f.readline()
                if self.original_msg:
                    print("size:", s.calcsize(self.fmt_str))
                    res = s.unpack(self.fmt_str,
                                   self.original_msg)
                    # print(res)
                    print("设备编号:{}, 数据长度:{}".format(res[1], res[2]))
                else:
                    break

    def decode_msg_print(self, msg_type, msg):
        print("-----报文内容-----")
        if msg_type == 0:
            res = s.unpack(self.fmt_str, msg)
            print(
                "[From Slave] 设备ID:{}, 陀螺仪数值:A:{:.2f},{:.2f},{:.2f} G:{:.2f}° {:.2f}° {:.2f}°".format(
                    res[0],
                    res[1] / 16384.0,
                    res[2] / 16384.0,
                    res[3] / 16384.0,
                    res[4] / 131.0 * 57.29577,
                    res[5] / 131.0 * 57.29577,
                    res[6] / 131.0 * 57.29577
                ))
            print("[From Slave] 水温:{:.2f}°C, 水深:{:.4f}cm, {:.4f}cm, 液面深度:{:.4f}cm,{:.4f}cm".format(res[7],
                                                                                                   res[8],
                                                                                                   res[9],
                                                                                                   res[10],
                                                                                                   res[11]))
            print("[From Slave] 状态:{}".format(
                [chr(res[12 + i]) if i == 0 else res[12+i] for i in range(10)]))

        elif msg_type == 1:
            device_id = s.unpack("H", msg[:2])[0]
            print("[From Slave] {}".format(msg[2:].decode("utf-8")))

        print("-----over-----")

    def decode_msg(self, msg_type, msg):
        if msg_type == 0:
            res = s.unpack(self.fmt_str, msg)
            return [
                res[0],
                res[1] / 16384.0,
                res[2] / 16384.0,
                res[3] / 16384.0,
                res[4] / 131.0 * 57.29577,
                res[5] / 131.0 * 57.29577,
                res[6] / 131.0 * 57.29577,
                res[7],
                res[8],
                res[9],
                res[10],
                res[11],
                res[12],
                res[13],
                res[14],
                res[15],
                res[16],
                res[17],
                res[18],
                res[19],
                res[20],
                res[21]
            ]

        elif msg_type == 1:
            return [
                msg[2:].decode("utf-8")]
