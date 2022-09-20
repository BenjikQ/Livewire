#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <memory>

#include <QImage>
#include <QMainWindow>

#include <opencv2/core/mat.hpp>

#include "cost_functions.hpp"
#include "graph.hpp"
#include "painter_options.hpp"
#include "path_item.hpp"

QT_BEGIN_NAMESPACE
class QEvent;
class QGraphicsScene;
class QLabel;
class QMouseEvent;
class QPoint;
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

    // Toolbar actions
    [[maybe_unused]] void showPointColorDialog();
    [[maybe_unused]] void showPathColorDialog();

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
    void updateLabel(const QPoint &position);

    void clickPoint(const QPoint &position);
    void drawPath(const QPoint &position);

    bool pointInImage(Point point) const;
    bool pointInImage(QPoint point) const;

private:
    bool m_drawing{ false };

    Ui::MainWindow *m_ui;
    QLabel *m_mouseCoordinatesIcon;
    QLabel *m_mouseCoordinatesLabel;
    QLabel *m_screenSizeIcon;
    QLabel *m_screenSizeLabel;

    QImage m_image;
    cv::Mat m_imageGray;
    std::unique_ptr<DiagonalGraph<CostFunction>> m_graph{ nullptr };

    PainterOptions m_painterOptions{ {}, Qt::red, Qt::red, 1, 4 };

    QRectF m_initialSceneRect{};
    QGraphicsScene *m_scene;
    QPoint m_mouseLastScenePosition{};
    PathItem *m_path{ nullptr };
    QUndoStack *m_undoStack;
    int m_numberOfPoints;
};

#endif  // MAIN_WINDOW_HPP
