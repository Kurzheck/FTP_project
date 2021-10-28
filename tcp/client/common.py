import os
import socket
from pathlib import Path
import sys
from util import *

from PySide2.QtWidgets import QLabel, QProgressBar, QPushButton, QRadioButton, QSpinBox, QTableWidget, QTextEdit, QWidget, QLineEdit
from PySide2.QtCore import QFile
from PySide2.QtUiTools import QUiLoader


class ClientWindow(QWidget):
    def __init__(self):
        super(ClientWindow, self).__init__()
        self.__load_ui()
        self.setFixedSize(700, 800)
        self.__init_widget()
        self.__init_status()
        self.__set_data()
        self.__init_connect()

    def __load_ui(self):
        loader = QUiLoader()
        path = os.fspath(Path(__file__).resolve().parent / "form.ui")
        ui_file = QFile(path)
        ui_file.open(QFile.ReadOnly)
        loader.load(ui_file, self)
        ui_file.close()

    def __init_widget(self):
        self.lineEdit_IP = self.findChild(QLineEdit, "lineEdit_IP")
        self.lineEdit_port = self.findChild(QLineEdit, "lineEdit_port")
        self.lineEdit_port.setMaxLength(10)
        self.lineEdit_username = self.findChild(QLineEdit, "lineEdit_username")
        self.lineEdit_password = self.findChild(QLineEdit, "lineEdit_password")
        self.radioButton_PASV = self.findChild(QRadioButton, "radioButton_PASV")
        self.radioButton_PORT = self.findChild(QRadioButton, "radioButton_PORT")
        self.pushButton_login = self.findChild(QPushButton, "pushButton_login")
        self.pushButton_logout = self.findChild(QPushButton, "pushButton_logout")
        self.label_status_content = self.findChild(QLabel, "label_status_content")
        self.lineEdit_cwd = self.findChild(QLineEdit, "lineEdit_cwd")
        self.pushButton_go = self.findChild(QPushButton, "pushButton_go")
        self.tableWidget_ls = self.findChild(QTableWidget, "tableWidget_ls")
        self.pushButton_mkdir = self.findChild(QPushButton, "pushButton_mkdir")
        self.pushButton_rm = self.findChild(QPushButton, "pushButton_rm")
        self.pushButton_mkdir = self.findChild(QPushButton, "pushButton_mkdir")
        self.pushButton_upload = self.findChild(QPushButton, "pushButton_upload")
        self.pushButton_download = self.findChild(QPushButton, "pushButton_download")
        self.progressBar = self.findChild(QProgressBar, "progressBar")
        self.textEdit_log = self.findChild(QTextEdit, "textEdit_log")

    def __init_status(self):
        self.lineEdit_IP.setText("166.111.80.66")
        self.lineEdit_port.setText("21")
        self.lineEdit_username.setText("anonymous")
        self.lineEdit_password.setText("password")
        self.label_status_content.setText("not connected")

    def __set_data(self):
        self.logs = []

    def __init_connect(self):
        self.pushButton_login.clicked.connect(self.Login);
        self.pushButton_logout.clicked.connect(lambda: self.Logout());

    def SendCmd(self, command):
        self.PrintLog(command)

    def RecvMsg(self):
        pass

    def Login(self):
        self.remote_IP = self.lineEdit_IP.text()
        self.remote_port = int(self.lineEdit_port.text())
        self.username = self.lineEdit_username.text()
        self.password = self.lineEdit_password.text()
        self.con_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        self.PrintLog("Username: " + str(self.username))
        self.PrintLog("Password: " + str(self.password))
        try:
            # TODO connect timeout
            self.con_socket.connect((self.remote_IP, self.remote_port))
            self.label_status_content.setText("connected")
        except Exception as e:
            self.PrintLog(e.__str__)
            self.label_status_content.setText("connection error")
        self.RecvMsg()
        self.USER_handler(self.username)
        self.PASS_handler(self.password)
        self.SYST_handler()
        self.TYPE_handler("I")
        self.PWD_handler()
        self.LIST_handler()

    def Logout(self):
        pass

    def PrintLog(self, msg):
        self.logs.append(msg)
        self.textEdit_log.setPlainText("\n".join(self.logs))

    def USER_handler(self, arg):
        self.SendCmd(cmd("USER", arg))
        self.RecvMsg()

    def PASS_handler(self, arg):
        self.SendCmd(cmd("PASS", arg))
        self.RecvMsg()

    def SYST_handler(self):
        self.SendCmd(cmd("SYST"))
        self.RecvMsg()

    def TYPE_handler(self, arg):
        self.SendCmd(cmd("TYPE", arg))
        self.RecvMsg()

    def PASV_handler(self):
        pass

    def PORT_handler(self):
        pass

    def QUIT_handler(self):
        pass

    def PWD_handler(self):
        pass

    def CWD_handler(self):
        pass

    def MKD_handler(self):
        pass

    def RMD_handler(self):
        pass

    def LIST_handler(self, arg=""):
        pass

    def STOR_handler(self):
        pass

    def RETR_handler(self):
        pass

    def RNFR_handler(self):
        pass

    def RNTO_handler(self):
        pass