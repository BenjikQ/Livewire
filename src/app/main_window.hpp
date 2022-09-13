#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QImage>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QEvent;
class QGraphicsLineItem;
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
    void setupUi();
    void setupSceneText();
    void setupSceneImage();
    void setupTextView();
    void setupImageView();
    void setupStatusBar();
    void setupActions(bool enabled);

    void createTextLabel(QLabel *&textLabel, const QString &text = "", int width = 0);
    void createIconLabel(QLabel *&iconLabel, const QString &iconPath);
    void updateLabel(const QPointF &position);

    void clickPoint(const QPointF &position);
    void drawEdge(const QPointF &position);

private:
    bool m_drawing{ false };

    Ui::MainWindow *m_ui;
    QLabel *m_mouseCoordinatesIcon;
    QLabel *m_mouseCoordinatesLabel;
    QLabel *m_screenSizeIcon;
    QLabel *m_screenSizeLabel;

    QImage m_image;

    QRectF m_initialSceneRect{};
    QGraphicsScene *m_scene;
    QGraphicsLineItem *m_line{ nullptr };
    QUndoStack *m_undoStack;
    int m_numberOfPoints;
};

#endif  // MAIN_WINDOW_HPP
