from PySide6.QtWidgets import QHBoxLayout, QPushButton, QSizePolicy
from PySide6.QtSvgWidgets import QSvgWidget
from PySide6.QtCore import Qt

import rc_images


class ButtonCell(QPushButton):
    def __init__(self, parent, is_mine):
        QPushButton.__init__(self, parent=parent)
        self.clicked.connect(self._on_clicked)
        self.setLayout(QHBoxLayout())
        self.layout().setContentsMargins(2, 2, 2, 2)

        self._is_mine = is_mine
        self._is_opened = False
        self._is_flagged = False
        self._neighbours = None
        self._num_surrounding_mines = 0
        self._widget_flag = None

    def _on_clicked(self):
        cells_opened = False
        if self.is_opened():
            if self.num_surrounding_mines() > 0:
                num_flags = 0
                for cell in self._neighbours:
                    if cell.is_flagged():
                        num_flags += 1
                if num_flags == self.num_surrounding_mines():
                    for cell in self._neighbours:
                        if cell.is_flagged() or cell.is_opened():
                            continue
                        cell.open()
                        cells_opened = True
        else:
            if not self.is_flagged():
                self.open()
                cells_opened = True

        if cells_opened:
            self.parent().new_cell_opened()

    def mousePressEvent(self, event):
        if event.button() == Qt.RightButton:
            if not self.is_opened():
                self._is_flagged = not self._is_flagged
                self.refresh_appearance()
                self.parent().count_flags(1 if self._is_flagged else -1)
        elif event.button() == Qt.LeftButton:
            if not self.is_opened():
                self.parent().show_surprise()

        return super(ButtonCell, self).mousePressEvent(event)

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.parent().show_normal()

        return super(ButtonCell, self).mouseReleaseEvent(event)

    def is_mine(self):
        return self._is_mine

    def is_flagged(self):
        return self._is_flagged

    def is_opened(self):
        return self._is_opened

    def num_surrounding_mines(self):
        return self._num_surrounding_mines

    def set_neighbours(self, neighbours):
        self._neighbours = neighbours
        self._num_surrounding_mines = 0
        for cell in neighbours:
            if cell.is_mine():
                self._num_surrounding_mines += 1

    def open(self):
        if self.is_opened():
            return
        self._is_opened = True

        if self.is_mine():
            self.refresh_appearance()
            self.parent().lose_game()
            return

        if self.num_surrounding_mines() == 0:
            for cell in self._neighbours:
                if cell.is_opened() or cell.is_flagged() or cell.is_mine():
                    continue
                cell.open()

        self.refresh_appearance()

    def reveal_after_losing(self):
        if self.is_opened():
            return
        if self.is_flagged() == self.is_mine():
            return

        self.setStyleSheet('border-style: inset; border-color: gray; border-width: 1px;'
                           'background-color: transparent;')
        widget_back = QSvgWidget(':/images/Zapfen-lineart.svg', self)
        widget_back.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.layout().addWidget(widget_back)

        if self.is_flagged():
            self.layout().takeAt(0).widget().deleteLater()   # delete the flag
            widget_back.setLayout(QHBoxLayout())
            widget_back.layout().setContentsMargins(0, 0, 0, 0)
            widget_cross = QSvgWidget(':/images/cross-mark.svg', widget_back)
            widget_back.layout().addWidget(widget_cross)

        if not self.is_mine():
            return

    def refresh_appearance(self):
        if self.is_opened():
            color = 'red' if self._is_mine else 'transparent'
            self.setStyleSheet(f'border-style: inset; border-color: gray; border-width: 1px;'
                               f'background-color: {color};')

            if self._widget_flag is not None:
                self._widget_flag.setVisible(False)

            if self._is_mine:
                widget_back = QSvgWidget(':/images/Zapfen-lineart.svg', self)
                widget_back.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
            else:
                if self._num_surrounding_mines == 0:
                    widget_back = None
                else:
                    widget_back = QSvgWidget(f':/images/number-{self._num_surrounding_mines}.svg', self)

            if widget_back is not None:
                self.layout().addWidget(widget_back)

        else:
            self.setStyleSheet('background-color: lightgray')

            if self._is_flagged:
                if self._widget_flag is None:
                    self._widget_flag = QSvgWidget(':/images/red_flag.svg', self)
                    self.layout().addWidget(self._widget_flag)
                else:
                    self._widget_flag.setVisible(True)
            elif self._widget_flag is not None:
                self._widget_flag.setVisible(False)
