import sys

import threading
from PySide2.QtCore import Qt
from PySide2.QtGui import QPixmap
from PySide2.QtWidgets import (
    QApplication,
    QCheckBox,
    QComboBox,
    QDateEdit,
    QDateTimeEdit,
    QDial,
    QDoubleSpinBox,
    QFontComboBox,
    QLabel,
    QLCDNumber,
    QLineEdit,
    QMainWindow,
    QProgressBar,
    QPushButton,
    QRadioButton,
    QSlider,
    QSpinBox,
    QTimeEdit,
    QHBoxLayout,
    QVBoxLayout,
    QWidget,
    QListWidget,
    QGridLayout,
    QMessageBox
)

import MySerialShip


class MainWindow(QMainWindow):

    # 是否为界面初始化
    init_gui = True

    # 串口模块
    serial_status = 0
    ss = MySerialShip.MySerialShip()
    curr_serial_port_name = ""

    # 组件相关
    status_num = 10
    status_widget_list = []
    command_read_widget_list = []
    command_input_widget_list = []

    communication_status = 0

    def __init__(self):
        super(MainWindow, self).__init__()

        self.setWindowTitle("My App")

        # 初始化类变量
        self.communication_lighting_widget = QLabel()
        self.serial_button = QPushButton("打开串口")
        self.send_button = QPushButton("Send")
        self.serial_comboBox = QComboBox()
        self.port_list = self.ss.check_comports()
        self.serial_label = QLabel()

        self.green_lighting = QPixmap(
            "D:\\workspace\\STM32\\project\\ReceivingEnd\\data\\green_lighting.png")
        self.red_lighting = QPixmap(
            "D:\\workspace\\STM32\\project\\ReceivingEnd\\data\\red_lighting.png")
        #

        # 加载组件
        serial_widget = self.add_serial_widget()
        status_widget = self.add_status_info_widget()
        command_widget = self.add_command_widget()
        command_input_widget = self.add_command_input_widget()
        send_widget = self.add_send_button()

        # 加载布局
        main_Vlayout = QVBoxLayout()
        main_Vlayout.addWidget(serial_widget)
        main_Vlayout.addWidget(status_widget)
        main_Vlayout.addWidget(command_widget)
        main_Vlayout.addWidget(command_input_widget)
        main_Vlayout.addWidget(send_widget)
        main_Vlayout.setMargin(1)
        main_Vlayout.setSpacing(1)

        receive_update = threading.Timer(0.02, self.update_receive_data)
        receive_update.setDaemon(True)
        receive_update.start()

        # self.setLayout(vbox)
        main_container = QWidget()
        main_container.setLayout(main_Vlayout)
        self.setCentralWidget(main_container)
        # self.setFixedSize(self.width(),self.height())
        # self.setMaximumSize(800,100)

    def closeEvent(self, event):
        reply = QMessageBox.question(self, 'Message',
                                     "Are you sure to quit?",
                                     QMessageBox.Yes |
                                     QMessageBox.No,
                                     QMessageBox.No)
        # 判断返回结果处理相应事项
        if reply == QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

    # 添加潜艇状态组件
    def add_status_info_widget(self):

        for i in range(self.status_num):
            self.status_widget_list.append(QLabel())

        self.status_widget_list[0].setText("MotorGear")
        self.status_widget_list[1].setText("Rudder0Angle")
        self.status_widget_list[2].setText("Rudder1Angle")
        self.status_widget_list[3].setText("Highbeam")
        self.status_widget_list[4].setText("Taillight")
        self.status_widget_list[5].setText("Headlight")
        self.status_widget_list[6].setText("WaterIn")
        self.status_widget_list[7].setText("WaterOut")
        self.status_widget_list[8].setText("SystemStatus0")
        self.status_widget_list[9].setText("SystemStatus1")

        Hlayout = QHBoxLayout()
        Hlayout.setSpacing(1)
        Hlayout.setMargin(1)
        for i in range(self.status_num):
            self.status_widget_list[i].setMaximumSize(60, 30)
            Hlayout.addWidget(self.status_widget_list[i])

        container = QWidget()
        container.setLayout(Hlayout)

        return container

    # 输入指令框组件
    def add_command_input_widget(self):

        for i in range(self.status_num):
            self.command_input_widget_list.append(QLineEdit())

        Hlayout = QHBoxLayout()
        Hlayout.setSpacing(1)
        Hlayout.setMargin(1)
        for i in range(self.status_num):
            self.command_input_widget_list[i].setAlignment(Qt.AlignCenter)
            self.command_input_widget_list[i].setMaximumSize(60, 30)
            Hlayout.addWidget(self.command_input_widget_list[i])

        container = QWidget()
        container.setLayout(Hlayout)

        return container

    # 显示潜艇状态组件
    def add_command_widget(self):

        for i in range(self.status_num):
            self.command_read_widget_list.append(QLineEdit())

        Hlayout = QHBoxLayout()
        Hlayout.setSpacing(1)
        Hlayout.setMargin(1)
        for i in range(self.status_num):
            self.command_read_widget_list[i].setAlignment(Qt.AlignCenter)
            self.command_read_widget_list[i].setReadOnly(True)
            self.command_read_widget_list[i].setMaximumSize(60, 30)
            Hlayout.addWidget(self.command_read_widget_list[i])

        container = QWidget()
        container.setLayout(Hlayout)

        return container

    # 添加指令操作布局
    def add_send_button(self):

        # 发送按钮
        self.send_button.setEnabled(True)
        self.send_button.setMaximumSize(60, 30)
        self.send_button.clicked.connect(self.btn_send_click)

        # 发送指示灯
        self.communication_lighting_widget.setPixmap(
            self.red_lighting.scaled(20, 20))
        self.communication_lighting_widget.setAlignment(
            Qt.AlignRight | Qt.AlignVCenter)
        self.communication_lighting_widget.setScaledContents(False)
        self.communication_lighting_widget.setContentsMargins(20, 1, 20, 1)

        # 发送状态指示
        info_1 = QLabel("等待的秒数:")
        info_1.adjustSize()
        waiting_second_widget = QLabel("2.12s")
        waiting_second_widget.setMaximumSize(60, 30)
        waiting_second_widget.setContentsMargins(1, 1, 10, 1)

        info_2 = QLabel("超时次数:")
        info_2.adjustSize()
        timeout_count_widget = QLabel("1")
        timeout_count_widget.setMaximumSize(60, 30)
        timeout_count_widget.setContentsMargins(1, 1, 10, 1)

        Hlayout = QHBoxLayout()
        Hlayout.setSpacing(1)
        Hlayout.setMargin(1)

        Hlayout.addWidget(self.send_button)
        Hlayout.addWidget(self.communication_lighting_widget)
        Hlayout.addWidget(info_1)
        Hlayout.addWidget(waiting_second_widget)
        Hlayout.addWidget(info_2)
        Hlayout.addWidget(timeout_count_widget)
        Hlayout.addStretch()

        container = QWidget()
        container.setLayout(Hlayout)
        return container

    def btn_send_click(self):
        input_value_list = self.get_command_input_widget_value()
        # 若输入的为空, 用read组件的替代
        for index, item in enumerate(self.command_read_widget_list):
            input_value = self.command_input_widget_list[index].text()
            read_value = item.text()
            if input_value == "":
                # 用read组件的替代
                self.command_input_widget_list[index] = read_value
                # 修改获得的数组
                input_value_list[index] = read_value
        self.change_communication_lighting()
        MySerialShip.msg_encode.change_control_signal(
            "MotorGear", ord(str(input_value_list[0]).strip()[:1]))
        MySerialShip.msg_encode.change_control_signal(
            "Rudder0Angle", int(input_value_list[1].strip()))
        MySerialShip.msg_encode.change_control_signal(
            "Rudder1Angle", int(input_value_list[2].strip()))
        MySerialShip.msg_encode.change_control_signal(
            "Highbeam", int(input_value_list[3].strip()))
        MySerialShip.msg_encode.change_control_signal(
            "Taillight", int(input_value_list[4].strip()))
        MySerialShip.msg_encode.change_control_signal(
            "Headlight", int(input_value_list[5].strip()))
        MySerialShip.msg_encode.change_control_signal(
            "WaterIn", int(input_value_list[6].strip()))
        MySerialShip.msg_encode.change_control_signal(
            "WaterOut", int(input_value_list[7].strip()))
        MySerialShip.msg_encode.change_control_signal(
            "SystemStatus0", int(input_value_list[8].strip()))
        MySerialShip.msg_encode.change_control_signal(
            "SystemStatus1", int(input_value_list[9].strip()))
        self.send_button.setEnabled(False)

    def get_command_input_widget_value(self):
        value_list = []
        for item in self.command_input_widget_list:
            # print(item.text())
            value_list.append(item.text())
        return value_list

    def change_communication_lighting(self):
        # 0->1
        if self.communication_status == 0:
            self.communication_lighting_widget.setPixmap(
                self.green_lighting.scaled(20, 20))
            self.communication_status = 1
        # 1->0
        elif self.communication_status == 1:
            self.communication_lighting_widget.setPixmap(
                self.red_lighting.scaled(20, 20))
            self.communication_status = 0

    def add_serial_widget(self):
        # 设置串口按钮
        self.serial_button.setEnabled(True)
        self.serial_button.setStyleSheet('QPushButton {color: black;}')
        self.serial_button.clicked.connect(self.serial_operation)

        # 设置串口选择按钮
        self.serial_comboBox = QComboBox()
        self.serial_comboBox.currentIndexChanged.connect(
            self.serial_comboBox_changed)

        # 串口说明
        self.serial_label = QLabel()
        self.serial_label.adjustSize()
        for port in self.port_list:  # 添加COMx
            self.serial_comboBox.addItem(list(port)[0])

        # 初始化一个值
        if len(self.port_list) > 0:  # 添加串口名称
            self.serial_comboBox.setCurrentIndex(0)
            self.curr_serial_port_name = list(self.port_list[0])[0]  # 名称
            self.serial_label.setText(list(self.port_list[0])[1])   # 描述

        Hlayout = QHBoxLayout()
        Hlayout.setSpacing(1)
        Hlayout.setMargin(1)

        Hlayout.addWidget(self.serial_button)
        Hlayout.addWidget(self.serial_comboBox)
        Hlayout.addWidget(self.serial_label)
        Hlayout.addStretch()

        container = QWidget()
        container.setLayout(Hlayout)
        return container

    def serial_comboBox_changed(self, index):
        # self.serial_comboBox.setCurrentIndex(index)
        self.serial_label.setText(list(self.port_list[index])[1])  # 描述
        self.curr_serial_port_name = list(self.port_list[index])[0]  # 名称
        print(self.curr_serial_port_name)

    def serial_operation(self):
        if self.serial_status == 0:
            self.ss.check_comports()
            self.ss.open_port(self.curr_serial_port_name, 9600, 1)
            # 启动接收发送线程
            self.ss.open_read_data_threading()
            self.serial_status = 1
            self.serial_button.setText("关闭串口")
            self.serial_button.setStyleSheet('QPushButton {color: red;}')
            self.serial_comboBox.setEnabled(False)

        elif self.serial_status == 1:
            # 关闭接收线程同时关闭串口
            self.ss.close_read_data_threading()
            self.ss.close_serial()
            self.serial_status = 0
            self.serial_button.setText("打开串口")
            self.serial_button.setStyleSheet('QPushButton {color: black;}')
            self.serial_comboBox.setEnabled(True)

    def update_receive_data(self):
        if len(MySerialShip.msg_list) > 0:
            msg = MySerialShip.msg_list.pop(0)
            MySerialShip.msg_decode.decode_msg_print(msg[0], msg[1])
            # msg[0]: 报文类别 msg[1]: 报文内容
            decode_result = MySerialShip.msg_decode.decode_msg(msg[0], msg[1])
            # print(decode_result)
            self.update_gui(msg[0], decode_result)
            self.send_button.setEnabled(True)

        receive_update = threading.Timer(0.02, self.update_receive_data)
        receive_update.setDaemon(True)
        receive_update.start()

    def update_gui(self, msg_type, decode_result):
        # print(decode_result[12:])
        if msg_type == 0:
            for i, w in enumerate(self.command_read_widget_list):
                # 第一个是档位
                if i == 0:
                    # print((decode_result[12 + i]))
                    w.setText(str(chr(decode_result[12 + i])))

                    # 使指令输入初始化一次
                    if self.init_gui:
                        self.command_input_widget_list[i].setText(
                            str(chr(decode_result[12 + i])))
                # 其它是数字
                else:
                    w.setText(str(decode_result[12 + i]))

                    # 使指令输入初始化一次
                    if self.init_gui:
                        self.command_input_widget_list[i].setText(
                            str(decode_result[12 + i]))
        elif msg_type == 1:
            pass

        # 标记已经经过初始化
        if self.init_gui == True:
            self.init_gui = False


if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = MainWindow()
    window.show()

    app.exec_()
