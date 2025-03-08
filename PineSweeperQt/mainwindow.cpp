#include "mainwindow.h"

#include <QActionGroup>
#include <QBoxLayout>
#include <QMenuBar>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    auto* const main_widget = new QWidget();
    setCentralWidget(main_widget);
    auto* const main_layout = new QVBoxLayout(main_widget);
    auto* const control_layout = new QHBoxLayout();
    main_layout->addLayout(control_layout);

    constexpr size_t ctrl_height = 100;
    led_mines_ = new LedDisplay(this, 3, ctrl_height);
    control_layout->addWidget(led_mines_);

    button_reset_ = new ButtonReset(main_widget);
    button_reset_->setFixedHeight(ctrl_height);
    button_reset_->setFixedWidth(ctrl_height);
    connect(button_reset_, &QPushButton::clicked, this, &MainWindow::newGame);
    control_layout->addWidget(button_reset_);

    led_time_ = new LedDisplay(this, 4, ctrl_height);
    control_layout->addWidget(led_time_);

    board_ = new Board(main_widget, *this);
    main_layout->addWidget(board_);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MainWindow::timerTick);

    auto* const game_menu = menuBar()->addMenu("Game");
    auto* const action_new_game = game_menu->addAction("New");
    connect(action_new_game, &QAction::triggered, this, &MainWindow::newGame);
    game_menu->addSeparator();
    auto* const action_easy = game_menu->addAction("Easy");
    action_easy->setCheckable(true);
    connect(action_easy, &QAction::triggered, this, [this](){ setGameSize("Easy"); });
    auto* const action_intermediate = game_menu->addAction("Intermediate");
    action_intermediate->setCheckable(true);
    connect(action_intermediate, &QAction::triggered, this, [this](){ setGameSize("Intermediate"); });
    auto* const action_expert = game_menu->addAction("Expert");
    action_expert->setCheckable(true);
    connect(action_expert, &QAction::triggered, this, [this](){ setGameSize("Expert"); });
    auto* const group_action = new QActionGroup(this);
    group_action->setExclusive(true);
    group_action->addAction(action_easy);
    group_action->addAction(action_intermediate);
    group_action->addAction(action_expert);
    game_menu->addSeparator();
    auto* const action_exit = game_menu->addAction("Exit");
    connect(action_exit, &QAction::triggered, this, &MainWindow::deleteLater);

    // Done
    newGame();
}

void MainWindow::setGameSize(const QString& label)
{
    QSettings().setValue("GameSize", label);
    newGame();
}

std::tuple<size_t, size_t, size_t> MainWindow::getGameSize()
{
    const auto game_str = QSettings().value("GameSize").toString();
    if (game_str == "Easy")
        return { 8, 8, 10 };
    else if (game_str == "Intermediate")
        return { 16, 16, 40 };
    else if (game_str == "Expert")
        return { 16, 30, 99 };
    else
        return { 1, 1, 0 };
}

void MainWindow::newCellOpened()
{
    if (!timer_started_ && !game_stopped_)
    {
        start_time_ = std::chrono::steady_clock::now();
        timer_->start(200);
        timer_started_ = true;
    }
}

void MainWindow::countFlags(int inc)
{
    num_flags_ += inc;
    led_mines_->setValue(num_mines_ - num_flags_);
}

void MainWindow::newGame()
{
    const auto [num_rows, num_columns, num_mines] = getGameSize();
    num_mines_ = num_mines;
    num_flags_ = 0;
    board_->newGame(num_rows, num_columns, num_mines);
    timer_started_ = false;
    button_reset_->showNormal();
    led_time_->setValue(0);
    led_mines_->setValue(num_mines_);
    timer_->stop();
    game_stopped_ = false;
}

void MainWindow::loseGame()
{
    button_reset_->loseGame();
    timer_->stop();
    game_stopped_ = true;
}

void MainWindow::winGame()
{
    button_reset_->winGame();
    timer_->stop();
    game_stopped_ = true;
}

void MainWindow::showSurprise()
{
    button_reset_->showSurprise();
}

void MainWindow::showNormal()
{
    button_reset_->showNormal();
}

void MainWindow::timerTick()
{
    const auto duration = std::chrono::steady_clock::now() - start_time_;
    led_time_->setValue(static_cast<size_t>(std::round(
        std::chrono::duration_cast<std::chrono::duration<double>>(duration).count())));
}
