# This Python file uses the following encoding: utf-8
import sys

from common import ClientWindow
from PySide2.QtWidgets import QApplication

if __name__ == "__main__":
    app = QApplication([])
    widget = ClientWindow()
    widget.show()
    sys.exit(app.exec_())
