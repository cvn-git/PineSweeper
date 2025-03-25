#include "led_display.h"

#include <QHBoxLayout>
#include <QPalette>

LedDisplay::LedDisplay(QWidget* parent, size_t num_digits, size_t height)
    : QWidget{ parent }
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto* const layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    setAutoFillBackground(true);
    setPalette(palette);

    for (size_t k = 0; k < num_digits; ++k)
    {
        digits_.push_back(new LedDigit(this, height));
        layout->addWidget(digits_.back());
    }
}

void LedDisplay::setValue(size_t value)
{
    const auto num_digits = digits_.size();
    if (value >= std::pow(10, num_digits))
        throw std::runtime_error("Invalid digit value");

    for (size_t k = 0; k < num_digits; ++k)
    {
        const size_t digit_value = value % 10;
        digits_[num_digits - k - 1]->setValue(digit_value);
        value = value / 10;
    }
}
