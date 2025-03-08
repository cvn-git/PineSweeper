#include "led_digit.h"

#include <QHBoxLayout>

#include <format>

LedDigit::LedDigit(QWidget* parent, size_t height)
    : QSvgWidget{ parent }
{
    setFixedHeight(height);
    setFixedWidth(static_cast<int>(std::round(height * 0.6)));

    auto* const layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setValue(0);
}

void LedDigit::setValue(size_t value)
{
    if ((value < 0) || (value > 9))
        throw std::runtime_error("Invalid digit value");

    load(QString::fromStdString(std::format(":/images/7Segment{}.svg", value)));
}
