# coding=utf-8
from MySerialShip import *
from MsgDecode import *

if __name__ == "__main__":

    # 初始化串口
    ss = MySerialShip()
    ss.check_comports()
    ss.open_port("COM6", 9600, 1)

    # 启动接收发送线程
    # ss.open_send_data_threading()
    ss.open_read_data_threading()

    # pre_path = "ReceivingEnd/"
    # filename = "2022-01-09 085512.XDat"

    # md = MsgDecode()
    # md.read_from_file(pre_path + filename)

    # me = MsgEncode(8)
    # me.make_encap_msg()
    while True:
        input_str = input()
        if input_str == "q":
            # ss.close_read_data_threading()
            # ss.close_send_data_threading()
            ss.close_read_data_threading()
            break
        elif input_str == "ic":
            msg_encode.change_control_signal("WaterIn", 0)
        elif input_str == "io":
            msg_encode.change_control_signal("WaterIn", 1)
        elif input_str == "oc":
            msg_encode.change_control_signal("WaterOut", 0)
        elif input_str == "oo":
            msg_encode.change_control_signal("WaterOut", 1)
        elif input_str == "d0":
            msg_encode.change_control_signal("Rudder0Angle", 0)
        elif input_str == "d1":
            msg_encode.change_control_signal("Rudder0Angle", 1)
