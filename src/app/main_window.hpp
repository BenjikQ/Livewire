#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>

#include "paint_area.hpp"

// clang-format off
namespace Ui { class MainWindow; }
// clang-format on

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void open();

private:
    Ui::MainWindow *m_ui;
    PaintArea *m_paintArea;
};

#endif  // MAIN_WINDOW_HPP
