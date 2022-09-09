#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QImage>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QGraphicsView;
class QLabel;
class QString;
QT_END_NAMESPACE

class DiagramScene;

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
    void setupStatusBar();

private:
    Ui::MainWindow *m_ui;
    QLabel *m_mouseCoordinatesLabel;

    QImage m_image;

    DiagramScene *m_scene;
};

#endif  // MAIN_WINDOW_HPP
