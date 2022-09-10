#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QImage>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QEvent;
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
    ~MainWindow() override;

    bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    void mouseMoveEvent(QMouseEvent *mouseEvent) override;
    void resizeEvent(QResizeEvent *resizeEvent) override;

private slots:
    [[maybe_unused]] void openImageFile();
    [[maybe_unused]] void saveImageFile();

private:
    void setupStatusBar();
    void setupIconsInStatusBar();
    void setupLabelsInStatusBar();

private:
    Ui::MainWindow *m_ui{ nullptr };
    QLabel *m_mouseCoordinatesIcon{ nullptr };
    QLabel *m_mouseCoordinatesLabel{ nullptr };
    QLabel *m_screenSizeIcon{ nullptr };
    QLabel *m_screenSizeLabel{ nullptr };

    QImage m_image{};

    DiagramScene *m_scene{ nullptr };
};

#endif  // MAIN_WINDOW_HPP
