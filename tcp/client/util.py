import os
import socket
from pathlib import Path
import sys

def cmd(command, arg=""):
    return str(command) + " " + arg + "\r\n"

def uni_size(size):
    if size[-1] == 'K':
        size = float(size[:-1]) * 1024
    elif size[-1] == 'M':
        size = float(size[:-1]) * 1024 * 1024
    elif size[-1] == 'G':
        size = float(size[:-1]) * 1024 * 1024 * 1024
    else:
        size = float(size)
    return int(size)