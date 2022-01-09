import struct as s

# please check https://docs.python.org/3/library/struct.html to know more details on this module.
# sample: https://www.journaldev.com/17401/python-struct-pack-unpack


class MsgDecode:
    def __init__(self) -> None:
        self.original_msg = ""
        self.decoded_msg = ""

    def read_from_file(self, filename):
        with open(filename, mode="rb") as f:
            while True:
                self.original_msg = f.readline()
                if self.original_msg:
                    print("size:", s.calcsize("<cHHhhhhhhfdfffBBBBBBBBc"))
                    res = s.unpack("<cHHhhhhhhfdfffBBBBBBBBc", self.original_msg)
                    # print(res)
                    print("设备编号:{}, 数据长度:{}".format(res[1], res[2]))
                else:
                    break
