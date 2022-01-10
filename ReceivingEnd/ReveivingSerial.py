# coding=utf-8
from MySerialShip import *
from MsgDecode import *

if __name__ == "__main__":

    # 初始化串口
    ss = MySerialShip()
    ss.check_comports()
    ss.open_port("COM6", 9600, 1)

    # 启动接收发送线程
    ss.open_send_data_threading()
    ss.open_read_data_threading()

    # pre_path = "ReceivingEnd/"
    # filename = "2022-01-09 085512.XDat"

    # md = MsgDecode()
    # md.read_from_file(pre_path + filename)

    #me = MsgEncode(8)
    #me.make_encap_msg()
    a = input()
    if a == "q":
      ss.close_read_data_threading()
      reading_threading_flag = False
      sending_threading_flag = False
