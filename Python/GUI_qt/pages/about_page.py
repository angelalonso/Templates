from PySide6.QtWidgets import QLabel, QTextEdit
from PySide6.QtGui import QPixmap
from PySide6.QtCore import Qt
from .base_page import BasePage

class AboutPage(BasePage):
    PAGE_NAME = "About"


    def build_ui(self):
        title = QLabel("About This Application")
        title.setStyleSheet("font-size: 24px; font-weight: bold;")

        long_text = QTextEdit()
        long_text.setReadOnly(True)
        long_text.setText(
        """This is an example of a scrollable long text widget.


        You can place documentation, license text, logs, or any long-form content here.


        """ * 10
        )

        image = QLabel()
        image.setAlignment(Qt.AlignCenter)
        image.setPixmap(QPixmap(128, 128))
        image.setStyleSheet("background: #cccccc;")
        image.setText("Image placeholder")

        self.layout.addWidget(title)
        self.layout.addWidget(image)
        self.layout.addWidget(long_text)
