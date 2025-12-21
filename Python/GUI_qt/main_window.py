from PySide6.QtWidgets import QMainWindow, QMenuBar, QStatusBar
from page_manager import PageManager
from pages.home_page import HomePage
from pages.settings_page import SettingsPage
from pages.about_page import AboutPage

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Modern Python App Template")
        self.resize(1000, 650)

        self.page_manager = PageManager()
        self.setCentralWidget(self.page_manager)

        self._register_pages()
        self._create_menus()
        self._create_status_bar()

        self.page_manager.show_page("Home")

    def _register_pages(self):
        self.page_manager.add_page(HomePage())
        self.page_manager.add_page(SettingsPage())
        self.page_manager.add_page(AboutPage())


    def _create_menus(self):
        menu_bar = QMenuBar(self)
        self.setMenuBar(menu_bar)

        # Navigation menu
        navigate_menu = menu_bar.addMenu("Navigate")
        for name in self.page_manager.page_names():
            action = navigate_menu.addAction(name)
            action.triggered.connect(
                lambda checked=False, n=name: self.page_manager.show_page(n)
            )

        # File menu
        file_menu = menu_bar.addMenu("File")
        file_menu.addAction("Exit", self.close)

        # Help menu
        help_menu = menu_bar.addMenu("Help")
        help_menu.addAction(
            "About", lambda: self.page_manager.show_page("About")
        )

    def _create_status_bar(self):
        status = QStatusBar(self)
        status.showMessage("Ready")
        self.setStatusBar(status)
