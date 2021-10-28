import os
import socket
from pathlib import Path
import sys

def cmd(command, arg=""):
    return command + " " + arg + "\r\n"