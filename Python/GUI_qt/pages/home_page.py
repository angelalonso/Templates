from PySide6.QtWidgets import QLabel, QPushButton
from PySide6.QtCore import Qt
from .base_page import BasePage

class HomePage(BasePage):
    PAGE_NAME = "Home"

    def build_ui(self):
        title = QLabel("Home Page")
        title.setAlignment(Qt.AlignCenter)
        title.setStyleSheet("font-size: 26px; font-weight: bold;")

        button = QPushButton("Click me")
        button.clicked.connect(lambda: print("Button clicked"))

        self.layout.addWidget(title)
        self.layout.addWidget(button)
        self.layout.addStretch()
