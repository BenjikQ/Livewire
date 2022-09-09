#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QImage>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QEvent;
class QGraphicsView;
class QLabel;
class QMouseEvent;
class QResizeEvent;
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

    bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    void mouseMoveEvent(QMouseEvent *mouseEvent) override;
    void resizeEvent(QResizeEvent *resizeEvent) override;

private slots:
    void open();

private:
    void setupStatusBar();

private:
    Ui::MainWindow *m_ui;
    QLabel *m_mouseCoordinatesIcon;
    QLabel *m_mouseCoordinatesLabel;
    QLabel *m_screenSizeIcon;
    QLabel *m_screenSizeLabel;

    QImage m_image;

    DiagramScene *m_scene;
};

#endif  // MAIN_WINDOW_HPP
