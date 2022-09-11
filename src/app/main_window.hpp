#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QImage>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QEvent;
class QGraphicsScene;
class QLabel;
class QMouseEvent;
class QPointF;
class QResizeEvent;
class QString;
class QUndoStack;
QT_END_NAMESPACE

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
    void closeEvent(QCloseEvent *closeEvent) override;

private slots:
    // File actions
    [[maybe_unused]] void openImageFile();
    [[maybe_unused]] void saveImageFile();
    [[maybe_unused]] void closeImageFile();

    // Edit actions
    [[maybe_unused]] void undo();
    [[maybe_unused]] void redo();

private:
    void setupIcons();
    void setupStatusBar();
    //    void setupScene();
    void setupIconsInStatusBar();
    void setupLabelsInStatusBar();

    void addPoint(const QPointF &position);

private:
    bool m_drawing{ false };

    Ui::MainWindow *m_ui{ nullptr };
    QLabel *m_mouseCoordinatesIcon{ nullptr };
    QLabel *m_mouseCoordinatesLabel{ nullptr };
    QLabel *m_screenSizeIcon{ nullptr };
    QLabel *m_screenSizeLabel{ nullptr };

    QImage m_image{};

    QRectF m_startSceneRect{};
    QGraphicsScene *m_scene{ nullptr };
    QUndoStack *m_undoStack{ nullptr };
};

#endif  // MAIN_WINDOW_HPP
