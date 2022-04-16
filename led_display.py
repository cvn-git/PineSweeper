from PySide6.QtWidgets import QWidget, QHBoxLayout, QSizePolicy
from PySide6.QtSvgWidgets import QSvgWidget

import rc_images


class Digit(QSvgWidget):
    def __init__(self, parent, height):
        super(Digit, self).__init__(parent=parent)
        self.setFixedHeight(height)
        self.setFixedWidth(int(round(height * 0.6)))

        self.setLayout(QHBoxLayout())
        self.layout().setContentsMargins(0, 0, 0, 0)
        self.layout().setSpacing(0)

        self.set_value(0)

    def set_value(self, value):
        if (value < 0) or (value > 9):
            raise SystemError('Invalid digit value')
        self.load(f':/images/7Segment{value}.svg')


class LedDisplay(QWidget):
    def __init__(self, parent, num_digits, height):
        super(LedDisplay, self).__init__(parent=parent)
        self.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        self.setLayout(QHBoxLayout())
        self.layout().setContentsMargins(0, 0, 0, 0)
        self.layout().setSpacing(0)

        self._digits = list()
        for k in range(num_digits):
            digit = Digit(self, height)
            self.layout().addWidget(digit)
            self._digits.append(digit)

    def set_value(self, value):
        num_digits = len(self._digits)
        if (value < 0) or (value >= (10 ** num_digits)):
            raise SystemError('Invalid digit value')
        for k in range(num_digits):
            digit_value = value % 10
            self._digits[num_digits - k - 1].set_value(digit_value)
            value = value // 10
