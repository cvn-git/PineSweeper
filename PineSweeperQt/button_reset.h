#ifndef BUTTON_RESET_H
#define BUTTON_RESET_H

#include <QPushButton>
#include <QSvgWidget>

class ButtonReset : public QPushButton
{
public:
    ButtonReset(QWidget* parent);
    void showNormal();
    void showSurprise();
    void loseGame();
    void winGame();

private:
    QSvgWidget* widget_smile_{ nullptr };
    QSvgWidget* widget_angel_{ nullptr };
    QSvgWidget* widget_win_{ nullptr };
    QSvgWidget* widget_surprise_{ nullptr };
    std::vector<QSvgWidget*> face_widgets_;
};

#endif  // BUTTON_RESET_H
