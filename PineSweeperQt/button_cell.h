#ifndef BUTTON_CELL_H
#define BUTTON_CELL_H

#include <QPushButton>
#include <QSvgWidget>

class Board;

class ButtonCell : public QPushButton
{
public:
    ButtonCell(Board& board, bool is_mine);
    void setNeighbours(std::vector<ButtonCell*>&& neighbours);
    void open();
    void revealAfterLosing();
    void refreshAppearance();
    bool isMine() const { return is_mine_; }
    bool isOpened() const { return is_opened_; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void onClicked();

private:
    Board& board_;
    const bool is_mine_;

    std::vector<ButtonCell*> neighbours_;
    QSvgWidget* widget_flag_{ nullptr };

    size_t num_surrounding_mines_{ 0 };
    bool is_opened_{ false };
    bool is_flagged_{ false };
};


#endif  // BUTTON_CELL_H
