#include "button_cell.h"

#include "board.h"

#include <QHBoxLayout>
#include <QMouseEvent>

ButtonCell::ButtonCell(Board& board, bool is_mine)
    : QPushButton{ &board }
    , board_{ board }
    , is_mine_{ is_mine }
{
    connect(this, &QPushButton::clicked, this, &ButtonCell::onClicked);

    auto* const layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);

}

void ButtonCell::onClicked()
{
    bool cells_opened = false;
    if (is_opened_)
    {
        if (num_surrounding_mines_ > 0)
        {
            size_t num_flags = 0;
            for (auto* const cell : neighbours_)
            {
                if (cell->is_flagged_)
                    ++num_flags;
            }

            if (num_flags == num_surrounding_mines_)
            {
                for (auto* const cell : neighbours_)
                {
                    if (cell->is_flagged_ || cell->is_opened_)
                        continue;

                    cell->open();
                    cells_opened = true;
                }
            }
        }
    }
    else if (!is_flagged_)
    {
        open();
        cells_opened = true;
    }

    if (cells_opened)
        board_.newCellOpened();
}

void ButtonCell::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        if (!is_opened_)
        {
            is_flagged_ = !is_flagged_;
            refreshAppearance();
            board_.countFlags(is_flagged_ ? 1 : -1);
        }
    }
    else if (event->button() == Qt::LeftButton)
    {
        if (!is_opened_)
            board_.showSurprise();
    }

    QPushButton::mousePressEvent(event);
}

void ButtonCell::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        board_.showNormal();

    QPushButton::mouseReleaseEvent(event);
}

void ButtonCell::setNeighbours(std::vector<ButtonCell*>&& neighbours)
{
    neighbours_ = std::move(neighbours);
    num_surrounding_mines_ = 0;
    for (auto* const cell : neighbours_)
    {
        if (cell->is_mine_)
            ++num_surrounding_mines_;
    }
}

void ButtonCell::open()
{
    if (is_opened_)
        return;
    is_opened_ = true;

    if (is_mine_)
    {
        refreshAppearance();
        board_.loseGame();
        return;
    }

    if (num_surrounding_mines_ == 0)
    {
        for (auto* const cell : neighbours_)
        {
            if (cell->is_opened_ || cell->is_flagged_ || cell->is_mine_)
                continue;
            cell->open();
        }
    }

    refreshAppearance();
}

void ButtonCell::revealAfterLosing()
{
    if (is_opened_ || (is_flagged_ == is_mine_))
        return;

    setStyleSheet("border-style: inset; border-color: gray; border-width: 1px;background-color: transparent;");
    auto* const widget_back = new QSvgWidget(":/images/Zapfen-lineart.svg");
    widget_back->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout()->addWidget(widget_back);

    if (is_flagged_)
    {
        layout()->takeAt(0)->widget()->deleteLater();   // delete the flag icon
        auto* const layout = new QHBoxLayout(widget_back);
        layout->setContentsMargins(0, 0, 0, 0);
        auto* const widget_cross = new QSvgWidget(":/images/cross-mark.svg");
        layout->addWidget(widget_cross);
    }
}

void ButtonCell::refreshAppearance()
{
    if (is_opened_)
    {
        const std::string color = is_mine_ ? "red" : "transparent";
        setStyleSheet(QString::fromStdString(std::format(
            "border-style: inset; border-color: gray; border-width: 1px;background-color: {};", color)));

        if (widget_flag_)
            widget_flag_->setVisible(false);

        QSvgWidget* widget_back = nullptr;
        if (is_mine_)
        {
            widget_back = new QSvgWidget(":/images/Zapfen-lineart.svg");
            widget_back->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        }
        else if (num_surrounding_mines_ > 0)
        {
            widget_back = new QSvgWidget(
                QString::fromStdString(std::format(":/images/number-{}.svg", num_surrounding_mines_)));
        }

        if (widget_back)
            layout()->addWidget(widget_back);
    }
    else
    {
        setStyleSheet("background-color: lightgray");

        if (is_flagged_)
        {
            if (!widget_flag_)
            {
                widget_flag_ = new QSvgWidget(":/images/red_flag.svg");
                layout()->addWidget(widget_flag_);
            }
            else
            {
                widget_flag_->setVisible(true);
            }
        }
        else if (widget_flag_)
        {
            widget_flag_->setVisible(false);
        }
    }
}
