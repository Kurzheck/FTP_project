import os
import socket
from pathlib import Path
import sys
import command

from PySide2.QtWidgets import QProgressBar, QPushButton, QRadioButton, QSpinBox, QTableWidget, QTextEdit, QWidget, QLineEdit
from PySide2.QtCore import QFile
from PySide2.QtUiTools import QUiLoader


class ClientWindow(QWidget):
    def __init__(self):
        super(ClientWindow, self).__init__()
        self.__load_ui()
        self.setFixedSize(700, 800)
        self.__init_widget()
        self.__init_status()
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
        self.lineEdit_username.setText("anonymous")

    def __init_connect(self):
        self.pushButton_login.clicked.connect(self.Login());
        self.pushButton_logout.clicked.connect(self.Logout());

    def Login(self):
        IP = self.lineEdit_IP.text()
        port = self.lineEdit_port.text()
        username = self.lineEdit_username.text()
        password = self.lineEdit_password.text()
        self.connfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def Logout(self):
        pass

