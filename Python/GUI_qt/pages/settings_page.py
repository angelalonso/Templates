from PySide6.QtWidgets import (
    QLabel,
    QCheckBox,
    QScrollArea,
    QWidget,
    QVBoxLayout,
)
from PySide6.QtCore import Qt
from .base_page import BasePage

class SettingsPage(BasePage):
    PAGE_NAME = "Settings"

    def build_ui(self):
        title = QLabel("Settings")
        title.setStyleSheet("font-size: 24px; font-weight: bold;")

        # Scrollable content
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)

        container = QWidget()
        scroll_layout = QVBoxLayout(container)

        for i in range(20):
            checkbox = QCheckBox(f"Enable option {i + 1}")
            scroll_layout.addWidget(checkbox)

            scroll_layout.addStretch()
            scroll.setWidget(container)

            self.layout.addWidget(title)
            self.layout.addWidget(scroll)
