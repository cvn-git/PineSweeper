#ifndef LED_DIGIT_H
#define LED_DIGIT_H

#include <QSvgWidget>

class LedDigit : public QSvgWidget
{
public:
    LedDigit(QWidget* parent, size_t height);

    void setValue(size_t value);
};

#endif  // LED_DIGIT_H
