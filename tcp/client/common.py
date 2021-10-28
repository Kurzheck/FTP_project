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
        self.con_status = False

    def __init_connect(self):
        self.pushButton_login.clicked.connect(self.Login)
        self.pushButton_logout.clicked.connect(self.Logout)

    def SendCmd(self, command):
        self.PrintLog(">> " + command)
        try:
            self.con_socket.send(command.encode())
        except Exception as e:
            self.PrintLog(e.__str__)
        

    def RecvMsg(self):
        msg = b"<< "
        while True:
            buf = self.con_socket.recv(1)
            msg += buf
            #if msg[-2:] == b"\r\n":
            if msg.endswith(b"\r\n"):
                break
        if isinstance(msg, bytes):
            msg = msg.decode()
        self.PrintLog(msg)
        return msg[3:]

    def Login(self):
        self.remote_IP = self.lineEdit_IP.text()
        self.remote_port = int(self.lineEdit_port.text())
        self.username = self.lineEdit_username.text()
        self.password = self.lineEdit_password.text()
        self.con_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        self.PrintLog("Server IP: " + str(self.remote_IP))
        self.PrintLog("Server port: " + str(self.remote_port))
        self.PrintLog("Username: " + str(self.username))
        self.PrintLog("Password: " + str(self.password) + "\n")
        try:
            # TODO connect timeout
            self.con_socket.connect((self.remote_IP, self.remote_port))
            self.label_status_content.setText("connected")
        except Exception as e:
            self.PrintLog(e.__str__)
            self.label_status_content.setText("connection error")
        self.RecvMsg()
        self.USER_handler(self.username)
        if self.PASS_handler(self.password):
            self.label_status_content.setText("logged in")
            self.SYST_handler()
            self.TYPE_handler("I")
            self.PWD_handler()
            self.LIST_handler()
        else:
            self.label_status_content.setText("login error")

    def Logout(self):
        self.QUIT_handler()
        self.con_status = False
        self.label_status_content.setText("not connected")
        self.logs = []
        self.textEdit_log.setPlainText("")

    def PrintLog(self, msg):
        self.logs.append(msg)
        self.textEdit_log.setPlainText("\n".join(self.logs))
        self.textEdit_log.moveCursor(self.textEdit_log.textCursor().End)

    def USER_handler(self, arg):
        self.SendCmd(cmd("USER", arg))
        return self.RecvMsg().startswith("230")

    def PASS_handler(self, arg):
        self.SendCmd(cmd("PASS", arg))
        return self.RecvMsg().startswith("230")

    def SYST_handler(self):
        self.SendCmd(cmd("SYST"))
        return self.RecvMsg().startswith("215")

    def TYPE_handler(self, arg):
        self.SendCmd(cmd("TYPE", arg))
        return self.RecvMsg().startswith("200")

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