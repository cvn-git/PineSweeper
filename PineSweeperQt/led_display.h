#ifndef LED_DISPLAY_H
#define LED_DISPLAY_H

#include "led_digit.h"

class LedDisplay : public QWidget
{
public:
    LedDisplay(QWidget* parent, size_t num_digits, size_t height);

    void setValue(size_t value);

private:
    std::vector<LedDigit*> digits_;
};

#endif  // LED_DISPLAY_H
