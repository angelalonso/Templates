from PySide6.QtWidgets import QStackedWidget

class PageManager(QStackedWidget):
    """Central registry and router for pages."""

    def __init__(self, parent=None):
        super().__init__(parent)
        self._pages = {}

    def add_page(self, page):
        self._pages[page.PAGE_NAME] = page
        self.addWidget(page)

    def show_page(self, page_name: str):
        self.setCurrentWidget(self._pages[page_name])

    def page_names(self):
        return list(self._pages.keys())
