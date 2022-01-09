# coding=utf-8
from MySerialShip import *
from MsgDecode import *


if __name__ == "__main__":
    ss = MySerialShip()
    ss.check_comports()
    ss.open_port("COM6", 9600, 1)

    #pre_path = "ReceivingEnd/"
    #filename = "2022-01-09 085512.XDat"
    #md = MsgDecode()
    #md.read_from_file(pre_path + filename)
