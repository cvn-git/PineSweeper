#include "button_reset.h"

#include <QHBoxLayout>

ButtonReset::ButtonReset(QWidget* parent)
    : QPushButton{ parent }
{
        setStyleSheet("background-color: lightgray");

        auto* const layout = new QHBoxLayout(this);
        layout->setContentsMargins(2, 2, 2, 2);

        widget_smile_ = new QSvgWidget(":/images/Smiley--DimpleMathew.svg");
        face_widgets_.push_back(widget_smile_);
        widget_angel_ = new QSvgWidget(":/images/face-angel.svg");
        face_widgets_.push_back(widget_angel_);
        widget_win_ = new QSvgWidget(":/images/Smiling-Sun-Face-In-Sunglasses.svg");
        face_widgets_.push_back(widget_win_);
        widget_surprise_ = new QSvgWidget(":/images/face-surprise.svg");
        face_widgets_.push_back(widget_surprise_);

        for (auto* const face : face_widgets_)
            layout->addWidget(face);
}

void ButtonReset::showNormal()
{
    for (auto* const face : face_widgets_)
        face->setVisible(false);
    widget_smile_->setVisible(true);
}

void ButtonReset::showSurprise()
{
    for (auto* const face : face_widgets_)
        face->setVisible(false);
    widget_surprise_->setVisible(true);
}

void ButtonReset::loseGame()
{
    for (auto* const face : face_widgets_)
        face->setVisible(false);
    widget_angel_->setVisible(true);
}

void ButtonReset::winGame()
{
    for (auto* const face : face_widgets_)
        face->setVisible(false);
    widget_win_->setVisible(true);
}
