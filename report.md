# 计算机网络 FTP实验报告

**软件92  周雨豪  2018013399**



## 1 实验环境

**操作系统：**Ubuntu 18.04 for server, Windows 10 for client

**内存：**8GB

**语言：**C for server, Python 3.6 for client, PySide2 for GUI



## 2 实现功能

### 2.1 Server

1. 实现的指令有 USER, PASS, SYST, TYPE, QUIT, PASV, PORT, MKD, CWD, PWD, RMD, RNFR, RNTO, LIST, STOR, RETR (作业要求的16个) 指令以及REST指令。
2. 用多线程实现对多客户端的支持。
3. 大文件传输。

### 2.2 Client

1. 实现的指令有 USER, PASS, SYST, TYPE, QUIT, PASV, PORT, MKD, CWD, PWD, RMD, RNFR, RNTO, LIST, STOR, RETR (作业要求的16个) 指令并能够与本实验的Server以及标准FTP服务器通信。
2. 支持浏览FTP服务器文件，创建、移除、重命名目录以及上传下载文件。
3. 实现了GUI界面。