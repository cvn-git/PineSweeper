#ifndef BOARD_H
#define BOARD_H

#include "button_cell.h"

#include <QWidget>

class MainWindow;

class Board : public QWidget
{
public:
    Board(QWidget* parent, MainWindow& main_window);

    void newGame(size_t num_rows, size_t num_columns, size_t num_mines);
    void newCellOpened();
    void countFlags(int inc);
    void showSurprise();
    void showNormal();
    void loseGame();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    ButtonCell* getCell(int row, int col) const;
    void winGame();
    void resizeBoard();

private:
    MainWindow& main_window_;

    std::vector<ButtonCell*> cells_;
    size_t num_rows_{ 0 };
    size_t num_columns_{ 0 };
};

#endif  // BOARD_H
