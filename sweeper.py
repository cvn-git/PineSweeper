import os
import sys
import random
import time
import importlib

if importlib.util.find_spec('PySide2'):
    from PySide2.QtWidgets import QApplication, QMainWindow, QHBoxLayout, QVBoxLayout, QSizePolicy
    from PySide2.QtWidgets import QWidget, QPushButton, QMessageBox, QAction, QActionGroup
    from PySide2.QtSvg import QSvgWidget
    from PySide2.QtCore import QTimer, QObject, QCoreApplication, QSettings
    import rc_images_pyside2
else:
    from PySide6.QtWidgets import QApplication, QMainWindow, QHBoxLayout, QVBoxLayout, QSizePolicy
    from PySide6.QtWidgets import QWidget, QPushButton, QMessageBox
    from PySide6.QtGui import QAction, QActionGroup
    from PySide6.QtSvgWidgets import QSvgWidget
    from PySide6.QtCore import QTimer, QObject, QCoreApplication, QSettings
    import rc_images_pyside6

from button_cell import ButtonCell
from led_display import LedDisplay


class Board(QWidget):
    def __init__(self, parent, top_app):
        QWidget.__init__(self, parent=parent)

        self._app = top_app
        self._cells = None
        self._num_rows = 0
        self._num_columns = 0

    def get_cell(self, row, col):
        if (row < 0) or (row >= self._num_rows) or (col < 0) or (col >= self._num_columns):
            return None
        else:
            return self._cells[row * self._num_columns + col]

    def new_cell_opened(self):
        self._app.new_cell_opened()

        # Check winning
        for cell in self._cells:
            if (not cell.is_mine()) and (not cell.is_opened()):
                return
        self.win_game()

    def count_flags(self, inc):
        self._app.count_flags(inc)

    def new_game(self, num_rows, num_columns, num_mines):
        num_cells = num_rows * num_columns
        if self._cells is not None:
            for cell in self._cells:
                cell.deleteLater()

        self._cells = list()
        self._num_rows = num_rows
        self._num_columns = num_columns

        random.seed()
        mine_set = random.sample(range(num_cells), num_mines)

        for k in range(num_cells):
            self._cells.append(ButtonCell(self, k in mine_set))

        for row in range(num_rows):
            for col in range(num_columns):
                neighbours = list()
                neighbours.append(self.get_cell(row - 1, col - 1))
                neighbours.append(self.get_cell(row - 1, col))
                neighbours.append(self.get_cell(row - 1, col + 1))
                neighbours.append(self.get_cell(row, col - 1))
                neighbours.append(self.get_cell(row, col + 1))
                neighbours.append(self.get_cell(row + 1, col - 1))
                neighbours.append(self.get_cell(row + 1, col))
                neighbours.append(self.get_cell(row + 1, col + 1))
                neighbours = [n for n in neighbours if n is not None]
                self.get_cell(row, col).set_neighbours(neighbours)

        self.resize_board()

        for cell in self._cells:
            cell.refresh_appearance()
            cell.show()

    def lose_game(self):
        for cell in self._cells:
            cell.reveal_after_losing()
            cell.setEnabled(False)
        self._app.lose_game()
        # QMessageBox.critical(self.parent(), 'You lose', 'You lose!')

    def win_game(self):
        for cell in self._cells:
            cell.setEnabled(False)
        self._app.win_game()
        # QMessageBox.critical(self.parent(), 'You win', 'You win!')

    def show_surprise(self):
        self._app.show_surprise()

    def show_normal(self):
        self._app.show_normal()

    def resize_board(self):
        board_width = self.geometry().width()
        board_height = self.geometry().height()
        # print(f'Board size: {board_width}x{board_height}')
        cell_size = min(board_width // self._num_columns, board_height // self._num_rows)
        start_x = int(round(board_width * 0.5 - self._num_columns * 0.5 * cell_size))
        start_y = int(round(board_height * 0.5 - self._num_rows * 0.5 * cell_size))
        for row in range(self._num_rows):
            for col in range(self._num_columns):
                cell = self._cells[row * self._num_columns + col]
                cell.setGeometry(start_x + col * cell_size, start_y + row * cell_size, cell_size, cell_size)

    def resizeEvent(self, event):
        if self._cells is not None:
            self.resize_board()
        return super(Board, self).resizeEvent(event)


class ButtonReset(QPushButton):
    def __init__(self, parent):
        QPushButton.__init__(self, parent=parent)
        self.setStyleSheet('background-color: lightgray')
        self.setLayout(QHBoxLayout())
        self.layout().setContentsMargins(2, 2, 2, 2)

        self._face_widgets = list()
        self._widget_smile = QSvgWidget(':/images/Smiley--DimpleMathew.svg', self)
        self._face_widgets.append(self._widget_smile)
        self._widget_angel = QSvgWidget(':/images/face-angel.svg', self)
        self._face_widgets.append(self._widget_angel)
        self._widget_win = QSvgWidget(':/images/Smiling-Sun-Face-In-Sunglasses.svg', self)
        self._face_widgets.append(self._widget_win)
        self._widget_surprise = QSvgWidget(':/images/face-surprise.svg', self)
        self._face_widgets.append(self._widget_surprise)

        for face in self._face_widgets:
            self.layout().addWidget(face)

    def show_normal(self):
        for face in self._face_widgets:
            face.setVisible(False)
        self._widget_smile.setVisible(True)

    def show_surprise(self):
        for face in self._face_widgets:
            face.setVisible(False)
        self._widget_surprise.setVisible(True)

    def lose_game(self):
        for face in self._face_widgets:
            face.setVisible(False)
        self._widget_angel.setVisible(True)

    def win_game(self):
        for face in self._face_widgets:
            face.setVisible(False)
        self._widget_win.setVisible(True)


class SweeperApp(QMainWindow):
    def __init__(self, *args, **kwargs):
        QMainWindow.__init__(self, *args, **kwargs)

        self._start_time = None
        self._timer_started = None
        self._game_stopped = None
        self._num_mines = None
        self._num_flags = None

        ctrl_height = 100

        main_widget = QWidget(self)
        self.setCentralWidget(main_widget)
        main_layout = QVBoxLayout(main_widget)

        control_layout = QHBoxLayout()

        main_layout.addLayout(control_layout)

        self._led_mines = LedDisplay(self, 3, ctrl_height)
        control_layout.addWidget(self._led_mines)

        self._button_reset = ButtonReset(main_widget)
        self._button_reset.setFixedHeight(ctrl_height)
        self._button_reset.setFixedWidth(ctrl_height)
        self._button_reset.clicked.connect(self.new_game)
        control_layout.addWidget(self._button_reset)

        self._led_time = LedDisplay(self, 4, ctrl_height)
        control_layout.addWidget(self._led_time)

        self._board = Board(main_widget, self)
        main_layout.addWidget(self._board)

        self._timer = QTimer(self)
        self._timer.timeout.connect(self.timer_tick)

        main_menu = self.menuBar()
        game_menu = main_menu.addMenu("Game")

        action_new_game = game_menu.addAction('New')
        action_new_game.triggered.connect(self.new_game)
        game_menu.addSeparator()
        action_easy = game_menu.addAction('Easy')
        action_easy.setCheckable(True)
        action_easy.triggered.connect(lambda: self.set_game_size('Easy'))
        action_intermediate = game_menu.addAction('Intermediate')
        action_intermediate.setCheckable(True)
        action_intermediate.triggered.connect(lambda: self.set_game_size('Intermediate'))
        action_expert = game_menu.addAction('Expert')
        action_expert.setCheckable(True)
        action_expert.triggered.connect(lambda: self.set_game_size('Expert'))
        group_action = QActionGroup(self)
        group_action.setExclusive(True)
        group_action.addAction(action_easy)
        group_action.addAction(action_intermediate)
        group_action.addAction(action_expert)
        game_menu.addSeparator()
        action_exit = game_menu.addAction('Exit')
        action_exit.triggered.connect(self.deleteLater)

        self.new_game()

    def set_game_size(self, game_str):
        QSettings().setValue('GameSize', game_str)
        self.new_game()

    @staticmethod
    def get_game_size():
        game_str = QSettings().value('GameSize')
        if game_str == 'Easy':
            return 8, 8, 10
        elif game_str == 'Intermediate':
            return 16, 16, 40
        elif game_str == 'Expert':
            return 16, 30, 99
        else:
            return 1, 1, 0

    def new_cell_opened(self):
        if (not self._timer_started) and (not self._game_stopped):
            self._start_time = time.perf_counter()
            self._timer.start(200)
            self._timer_started = True

    def count_flags(self, inc):
        self._num_flags += inc
        self._led_mines.set_value(self._num_mines - self._num_flags)

    def new_game(self):
        num_rows, num_columns, num_mines = self.get_game_size()
        self._num_mines = num_mines
        self._num_flags = 0
        self._board.new_game(num_rows, num_columns, num_mines)
        self._timer_started = False
        self._button_reset.show_normal()
        self._led_time.set_value(0)
        self._led_mines.set_value(self._num_mines - self._num_flags)
        self._timer.stop()
        self._game_stopped = False

    def lose_game(self):
        self._button_reset.lose_game()
        self._timer.stop()
        self._game_stopped = True

    def win_game(self):
        self._button_reset.win_game()
        self._timer.stop()
        self._game_stopped = True

    def show_surprise(self):
        self._button_reset.show_surprise()

    def show_normal(self):
        self._button_reset.show_normal()

    def timer_tick(self):
        self._led_time.set_value(int(round((time.perf_counter() - self._start_time) * 1)))


if __name__ == '__main__':
    app = QApplication(sys.argv)

    # For QSettings
    QCoreApplication.setOrganizationName('CVN')
    QCoreApplication.setOrganizationDomain('github.com')
    QCoreApplication.setApplicationName('PineSweeper')

    my_app = SweeperApp()
    my_app.setWindowTitle('PineSweeper')
    my_app.show()
    my_app.resize(1600, 900)
    sys.exit(app.exec_())
