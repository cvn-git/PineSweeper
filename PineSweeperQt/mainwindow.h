#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "board.h"
#include "button_reset.h"
#include "led_display.h"

#include <QMainWindow>
#include <QTimer>

#include <chrono>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

    void newGame();
    void newCellOpened();
    void countFlags(int inc);
    void loseGame();
    void winGame();
    void showSurprise();
    void showNormal();

private:
    void timerTick();
    void setGameSize(const QString& label);

    static std::tuple<size_t, size_t, size_t> getGameSize();

private:
    LedDisplay* led_mines_{ nullptr };
    ButtonReset* button_reset_{ nullptr };
    LedDisplay* led_time_{ nullptr };
    Board* board_{ nullptr };
    QTimer* timer_{ nullptr };

    std::chrono::steady_clock::time_point start_time_;
    size_t num_mines_{ 0 };
    int num_flags_{ 0 };
    bool timer_started_{ false };
    bool game_stopped_{ false };
};

#endif // MAINWINDOW_H
