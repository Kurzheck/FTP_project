import os
from pathlib import Path
import sys

from PySide2.QtWidgets import QWidget
from PySide2.QtCore import QFile
from PySide2.QtUiTools import QUiLoader


class ClientWindow(QWidget):
    def __init__(self):
        super(ClientWindow, self).__init__()
        self.load_ui()

    def load_ui(self):
        loader = QUiLoader()
        path = os.fspath(Path(__file__).resolve().parent / "form.ui")
        ui_file = QFile(path)
        ui_file.open(QFile.ReadOnly)
        loader.load(ui_file, self)
        ui_file.close()