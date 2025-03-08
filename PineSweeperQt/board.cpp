#include "board.h"

#include "mainwindow.h"

#include <random>
#include <set>

Board::Board(QWidget* parent, MainWindow& main_window)
    : QWidget{ parent }
    , main_window_{ main_window }
{
}
ButtonCell* Board::getCell(int row, int col) const
{
    if ((row < 0) || (row >= num_rows_) || (col < 0) || (col >= num_columns_))
        return nullptr;
    else
        return cells_[row * num_columns_ + col];
}

void Board::newCellOpened()
{
    main_window_.newCellOpened();

    // Check winning
    for (auto* const cell : cells_)
    {
        if (!cell->isMine() && !cell->isOpened())
            return;
    }
    winGame();
}

void Board::countFlags(int inc)
{
    main_window_.countFlags(inc);
}

void Board::newGame(size_t num_rows, size_t num_columns, size_t num_mines)
{
    for (auto* const cell : cells_)
        cell->deleteLater();

    const auto num_cells = num_rows * num_columns;
    if (num_cells < num_mines)
        throw std::runtime_error("Too many mines");
    cells_.clear();
    num_rows_ = num_rows;
    num_columns_ = num_columns;

    std::vector<size_t> indices(num_cells, 0);
    std::iota(indices.begin(), indices.end(), 0);
    std::random_device rd;
    std::shuffle(indices.begin(), indices.end(), std::mt19937(rd()));
    std::set<size_t> mine_indices;
    for (size_t k = 0; k < num_mines; ++k)
        mine_indices.insert(indices[k]);

    for (size_t k = 0; k < num_cells; ++k)
        cells_.push_back(new ButtonCell(*this, mine_indices.count(k) != 0));

    for (size_t row = 0; row < num_rows; ++row)
    {
        for (size_t col = 0; col < num_columns; ++col)
        {
            std::vector<ButtonCell*> neighbours;
            const auto append_neighbour = [&](ButtonCell* cell) {
                if (cell)
                    neighbours.push_back(cell);
            };
            append_neighbour(getCell(row - 1, col - 1));
            append_neighbour(getCell(row - 1, col));
            append_neighbour(getCell(row - 1, col + 1));
            append_neighbour(getCell(row, col - 1));
            append_neighbour(getCell(row, col + 1));
            append_neighbour(getCell(row + 1, col - 1));
            append_neighbour(getCell(row + 1, col));
            append_neighbour(getCell(row + 1, col + 1));
            getCell(row, col)->setNeighbours(std::move(neighbours));
        }
    }

    resizeBoard();

    for (auto* const cell : cells_)
    {
        cell->refreshAppearance();
        cell->show();
    }
}

void Board::loseGame()
{
    for (auto* const cell : cells_)
    {
        cell->revealAfterLosing();
        cell->setEnabled(false);
    }
    main_window_.loseGame();
}

void Board::winGame()
{
    for (auto* const cell : cells_)
    {
        cell->revealAfterLosing();
        cell->setEnabled(false);
    }
    main_window_.winGame();
}

void Board::showSurprise()
{
    main_window_.showSurprise();
}

void Board::showNormal()
{
    main_window_.showNormal();
}

void Board::resizeBoard()
{
    const size_t board_width = geometry().width();
    const size_t board_height = geometry().height();
    const size_t cell_size = std::min(board_width / num_columns_, board_height / num_rows_);
    const auto start_x = static_cast<size_t>(std::round(board_width * 0.5 - num_columns_ * 0.5 * cell_size));
    const auto start_y = static_cast<size_t>(std::round(board_height * 0.5 - num_rows_ * 0.5 * cell_size));
    for (size_t row = 0; row < num_rows_; ++row)
    {
        for (size_t col = 0; col < num_columns_; ++col)
            getCell(row, col)->setGeometry(
                start_x + col * cell_size, start_y + row * cell_size, cell_size, cell_size);
    }
}

void Board::resizeEvent(QResizeEvent *event)
{
    if (!cells_.empty())
        resizeBoard();

    QWidget::resizeEvent(event);
}
