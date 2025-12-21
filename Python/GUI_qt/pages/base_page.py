from PySide6.QtWidgets import QWidget, QVBoxLayout

class BasePage(QWidget):
    PAGE_NAME = "Base"


    def __init__(self, parent=None):
        super().__init__(parent)
        self.layout = QVBoxLayout(self)
        self.layout.setSpacing(12)
        self.build_ui()


    def build_ui(self):
        """Override in subclasses."""
        pass
