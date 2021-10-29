import os
import socket
import re
from pathlib import Path
import sys
from util import *

from PySide2.QtWidgets import QLabel, QProgressBar, QPushButton, QRadioButton, QSpinBox, QTableWidget, QTextEdit, QWidget, QLineEdit, QInputDialog, QTableWidgetItem, QHeaderView, QAbstractItemView
from PySide2.QtCore import QFile
from PySide2.QtUiTools import QUiLoader


class ClientWindow(QWidget):

    PASV_MODE = 0
    PORT_MODE = 1

    def __init__(self):
        super(ClientWindow, self).__init__()
        self.__load_ui()
        self.__init_widget()
        self.__init_status()
        self.__init_data()
        self.__init_connect()

    def __load_ui(self):
        loader = QUiLoader()
        path = os.fspath(Path(__file__).resolve().parent / "form.ui")
        ui_file = QFile(path)
        ui_file.open(QFile.ReadOnly)
        loader.load(ui_file, self)
        ui_file.close()
        self.setFixedSize(830, 1400)

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
        self.pushButton_rmdir = self.findChild(QPushButton, "pushButton_rmdir")
        self.pushButton_upload = self.findChild(QPushButton, "pushButton_upload")
        self.pushButton_download = self.findChild(QPushButton, "pushButton_download")
        self.pushButton_rename = self.findChild(QPushButton, "pushButton_rename")
        self.progressBar = self.findChild(QProgressBar, "progressBar")
        self.textEdit_log = self.findChild(QTextEdit, "textEdit_log")
        self.pushButton_clear_log = self.findChild(QPushButton, "pushButton_clear_log")

    def __init_status(self):
        self.lineEdit_IP.setText("166.111.80.66")
        self.lineEdit_port.setText("21")
        self.lineEdit_username.setText("anonymous")
        self.lineEdit_password.setText("password")
        self.label_status_content.setText("not connected")
        self.radioButton_PASV.setChecked(True)

        headers = ["Type", "Permission", "Num", "User", "Group", "Size", "Updated", "Name"]
        self.tableTypeCol = 0
        self.tablePermissionCol = 1
        self.tableNumCol = 2
        self.tableUserCol = 3
        self.tableGroupCol = 4
        self.tableSizeCol = 5
        self.tableUpdatedCol = 6
        self.tableNameCol = 7
        self.tableWidget_ls.setColumnCount(len(headers))
        for idx, header in enumerate(headers):
            headerItem = QTableWidgetItem(header)
            headerItem.setTextColor("#00557f")
            font = headerItem.font()
            font.setBold(True)
            #headerItem.setFont(font)
            self.tableWidget_ls.setHorizontalHeaderItem(idx, headerItem)
        self.tableWidget_ls.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeToContents)
        self.tableWidget_ls.verticalHeader().setSectionResizeMode(QHeaderView.ResizeToContents)
        self.tableWidget_ls.setSelectionMode(QAbstractItemView.SingleSelection)
        self.tableWidget_ls.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.tableWidget_ls.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.tableWidget_ls.setShowGrid(False);
        self.tableWidget_ls.verticalHeader().setHidden(True)

    def __init_data(self):
        self.logs = []
        self.user_status = False
        self.OnStatusChange()
        self.mode = self.PASV_MODE
        self.has_data_con = False
        self.cwd = ""
        self.data_IP = ""
        self.data_port = -1
        self.data_socket = None
        self.listen_socket = None

    def __init_connect(self):
        self.pushButton_login.clicked.connect(self.Login)
        self.pushButton_logout.clicked.connect(self.Logout)
        self.radioButton_PASV.toggled.connect(self.SelectPASV)
        self.radioButton_PORT.toggled.connect(self.SelectPORT)
        self.pushButton_clear_log.clicked.connect(self.ClearLog)
        self.pushButton_go.clicked.connect(self.ChangeDir)
        self.pushButton_mkdir.clicked.connect(self.MakeDir)
        self.pushButton_rmdir.clicked.connect(self.RemoveDir)
        self.pushButton_rename.clicked.connect(self.Rename)
        self.tableWidget_ls.itemSelectionChanged.connect(self.TableUpdate)

############################################   util   ############################################

    def OnStatusChange(self):
        flag = self.user_status
        self.lineEdit_cwd.setEnabled(flag)
        self.pushButton_go.setEnabled(flag)
        self.pushButton_mkdir.setEnabled(flag)
        self.pushButton_rmdir.setEnabled(flag)
        self.pushButton_upload.setEnabled(flag)
        self.pushButton_download.setEnabled(flag)
        self.pushButton_rename.setEnabled(flag)
        self.pushButton_logout.setEnabled(flag)
        self.pushButton_login.setEnabled(not flag)
        self.lineEdit_IP.setEnabled(not flag)
        self.lineEdit_port.setEnabled(not flag)
        self.lineEdit_username.setEnabled(not flag)
        self.lineEdit_password.setEnabled(not flag)

    def PrintLog(self, msg):
        self.logs.append(msg)
        self.textEdit_log.setPlainText("\n".join(self.logs))
        self.textEdit_log.moveCursor(self.textEdit_log.textCursor().End)

    def ClearLog(self):
        self.logs = []
        self.textEdit_log.setPlainText("")

    def SendCmd(self, command):
        self.PrintLog(">> " + command)
        try:
            self.con_socket.send(command.encode())
        except Exception as e:
            self.PrintLog(e.__str__())

    def RecvRes(self):
        res = b""
        while True:
            buf = self.con_socket.recv(1)
            res += buf
            if res.endswith(b"\r\n"):
                break
        if isinstance(res, bytes):
            res = res.decode()
        self.PrintLog("<< " + res)
        code, msg = int(res[:3]), res[4:].replace("\r\n", "")
        return (code, msg)

    def SendData(self):
        pass

    def RecvData(self, arg=None):
        if arg:
            pass
        else:
            data = b''
            while True:
                buf = self.data_socket.recv(1000)
                if not buf:
                    break
                else:
                    data += buf
            if isinstance(data, bytes):
                data = data.decode()
            return data

    def DataSetMode(self):
        if self.mode == self.PASV_MODE:
            return self.PASV_handler()
        elif self.mode == self.PORT_MODE:
            return self.PORT_handler()

    def DataConnect(self):
        if self.mode == self.PASV_MODE:
            self.data_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.data_socket.connect((self.data_IP, self.data_port))
            return self.RecvRes()
        elif self.mode == self.PORT_MODE:
            assert(self.listen_socket)
            self.data_socket = self.listen_socket.accept()[0]
            return self.RecvRes()
        return False

############################################   slot   ############################################

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
            self.PrintLog(e.__str__())
            self.label_status_content.setText("connection error")
        self.RecvRes()
        self.USER_handler(self.username)
        if self.PASS_handler(self.password):
            self.label_status_content.setText("logged in")
            self.user_status = True
            self.OnStatusChange()
            self.SYST_handler()
            self.TYPE_handler("I")
            if self.PWD_handler():
                self.lineEdit_cwd.setText(self.cwd)
            self.LIST_handler(self.cwd)
        else:
            self.label_status_content.setText("login error")

    def Logout(self):
        if self.user_status:
            self.QUIT_handler()
        self.user_status = False
        self.OnStatusChange()
        self.has_data_con = False
        self.cwd = ""
        self.data_IP = ""
        self.data_port = -1
        self.data_socket = None
        self.listen_socket = None
        self.label_status_content.setText("not connected")
        self.lineEdit_cwd.setText("")

    def SelectPASV(self):
        self.mode = self.PASV_MODE
        self.radioButton_PORT.setChecked(False)

    def SelectPORT(self):
        self.mode = self.PORT_MODE
        self.radioButton_PASV.setChecked(False)

    def ChangeDir(self):
        tmp_dir = self.lineEdit_cwd.text()
        try:
            assert(self.CWD_handler(tmp_dir))
            assert(self.PWD_handler())
            assert(self.LIST_handler())
        except Exception as e:
            self.PrintLog(e.__str__())
        self.lineEdit_cwd.setText(self.cwd)

    def MakeDir(self):
        text, ok = QInputDialog.getText(self, "New Folder", "New Folder Name:", QLineEdit.Normal, "untitled")
        if text and ok:
            self.MKD_handler(text)

    def RemoveDir(self):
        rm_dir = "/a"
        self.RMD_handler(rm_dir)

    def Rename(self):
        text, ok = QInputDialog.getText(self, "Rename", "New Name:", QLineEdit.Normal, "untitled")
        if text and ok:
            if self.RNFR_handler():
                self.RNTO_handler(text)
            # LIST

    def TableUpdate(self):
        selected = self.tableWidget_ls.selectedItems()
        if len(selected):
            self.selectedFileItems = [_.text() for _ in selected]
        # filetype
        ftype = self.selectedFileItems[self.tableTypeCol]
        if ftype == "d":
            self.pushButton_rmdir.setEnabled(True)
        else:
            self.pushButton_rmdir.setEnabled(False)

############################################   handler   ############################################

    def USER_handler(self, arg):
        self.SendCmd(cmd("USER", arg))
        return self.RecvRes()[0] == 230

    def PASS_handler(self, arg):
        self.SendCmd(cmd("PASS", arg))
        return self.RecvRes()[0] == 230

    def SYST_handler(self):
        self.SendCmd(cmd("SYST"))
        return self.RecvRes()[0] == 215

    def TYPE_handler(self, arg):
        self.SendCmd(cmd("TYPE", arg))
        return self.RecvRes()[0] == 200

    def PASV_handler(self):
        self.SendCmd(cmd("PASV"))
        code, msg = self.RecvRes()
        seg = re.findall(re.compile(r'[(](.*)[)]', re.S), msg)[0].split(',')
        self.data_IP = ".".join(seg[:-2])
        self.data_port = int(seg[-2]) * 256 + int(seg[-1])
        self.data_socket = None
        return code == 227

    def PORT_handler(self):
        local = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        local.connect(("8.8.8.8", 80))
        IP =  local.getsockname()[0]
        self.listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.listen_socket.bind(("0.0.0.0", 0))
        self.listen_socket.listen(1)
        port = self.listen_socket.getsockname()[1]
        self.SendCmd(cmd("PORT", ",".join(IP.split(".")) + "," + str(port//256) + "," + str(port%256)))
        code, msg = self.RecvRes()
        return code == 227

    def QUIT_handler(self):
        self.SendCmd(cmd("QUIT"))
        return self.RecvRes()[0] == 221

    def PWD_handler(self):
        self.SendCmd(cmd("PWD"))
        code, msg = self.RecvRes()
        if code == 257:
            self.cwd = msg.replace("\"", "")
            return True
        return False

    def CWD_handler(self, arg):
        self.SendCmd(cmd("CWD", arg))
        return self.RecvRes()[0] == 250

    def MKD_handler(self, arg):
        self.SendCmd(cmd("MKD", arg))
        return self.RecvRes()[0] == 250

    def RMD_handler(self, arg):
        self.SendCmd(cmd("RMD", arg))
        return self.RecvRes()[0] == 250

    def LIST_handler(self, arg=""):
        if not self.DataSetMode():
            return False
        self.SendCmd(cmd("LIST", arg))
        code, res = self.DataConnect()
        if code != 150:
            return False
        data = self.RecvData()
        for i in range(self.tableWidget_ls.rowCount()):
            self.tableWidget_ls.removeRow(0)
        data.replace("\r", "")
        for i, line in enumerate(data.split("\n")):
            seg = line.split()
            if len(seg) == 0:
                continue
            print(seg)
            type = seg[0][0]
            seg[0] = seg[0][1:]
            row_data = []
            for k in range(5):
                row_data.append(seg[k])
            row_data.append(" ".join(seg[5:8]))
            row_data.append(" ".join(seg[8:]))
            row_data[1] = row_data[1][1:]
            self.tableWidget_ls.insertRow(i)
            for j, cell_data in enumerate(row_data):
                self.tableWidget_ls.setItem(i, j, QTableWidgetItem(cell_data))

            
        self.RecvRes()
        

    def STOR_handler(self, arg):
        pass

    def RETR_handler(self, arg):
        pass

    def RNFR_handler(self, arg):
        self.SendCmd(cmd("RNFR", arg))
        return self.RecvRes()[0] == 350

    def RNTO_handler(self, arg):
        self.SendCmd(cmd("RNFR", arg))
        return self.RecvRes()[0] == 250