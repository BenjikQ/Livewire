#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <memory>
#include <unordered_set>
#include <vector>

#include <QColor>
#include <QImage>
#include <QMainWindow>

#include <opencv2/core/mat.hpp>

#include "comparison_display.hpp"
#include "cost_functions.hpp"
#include "graph.hpp"
#include "painter_options.hpp"
#include "path_item.hpp"
#include "presave_dialog.hpp"
#include "selection_layer_item.hpp"

QT_BEGIN_NAMESPACE
class QEvent;
class QGraphicsScene;
class QGraphicsVideoItem;
class QLabel;
class QMediaPlayer;
class QMouseEvent;
class QPoint;
class QResizeEvent;
class QString;
class QUndoStack;
QT_END_NAMESPACE

class PointItem;
struct PathSequence;

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
    [[maybe_unused]] void openFile();
    [[maybe_unused]] void saveFile();
    [[maybe_unused]] void saveOutlines();
    [[maybe_unused]] void loadOutlines();
    [[maybe_unused]] void closeFile();

    // Edit actions
    [[maybe_unused]] void undo();
    [[maybe_unused]] void redo();

    // Toolbar actions
    [[maybe_unused]] void showPointColorDialog();
    [[maybe_unused]] void showPathColorDialog();
    [[maybe_unused]] void compareJaccard();

    // Other actions
    [[maybe_unused]] void closePath();
    [[maybe_unused]] void deletePoint();

    void scalingTime(qreal x);
    void animationFinished();
    void zoomIn();
    void zoomOut();

    void pauseAndPlayMovie();
    void drawOnVideoFrame();
    void nextFrame();
    void previousFrame();

    // Debug actions
    [[maybe_unused]] void dumpSceneIds() const;
    [[maybe_unused]] void showSelectedSeq();

private:
    void openImageFile(const QString &filePath);
    void openVideoFile(const QString &filePath);

    void saveOutlines(const QString &filePath);
    void loadOutlines(const QString &filePath);

    void setupUi();
    void setupSceneText();
    void setupSceneImage();
    void setupSceneVideo();
    void setupTextView();
    void setupImageView();
    void setupVideoView();
    void setupStatusBar();
    void setupActions(bool enabled);

    void createTextLabel(QLabel *&textLabel, const QString &text = "", int width = 0);
    void createIconLabel(QLabel *&iconLabel, const QString &iconPath);
    void updateLabel(const QPoint &position);

    void clickPoint(const QPoint &position, bool final = false);
    void drawPath(const QPoint &position);
    void fillFromPoint(const QPoint &position);
    void trySelectPoint(const QPoint &position);

    bool pointInScene(Point point) const;
    bool pointInScene(QPoint point) const;
    QPoint getLastPoint() const;
    QPoint getFirstPoint() const;
    std::unordered_set<QPoint> pointsFromScene() const;
    QImage imageFromScene(SaveOpts opts);
    PointItem *clickedPoint(int x, int y);
    PathSequence getPathSequence(int pointId);
    void sceneReindex();

    template <typename T>
    std::vector<T *> sceneItems(int beginIndex, int count = std::numeric_limits<int>::max() / 2);

    QList<QPoint> generatePath(const QPoint &start, const QPoint &end) const;

private:
    constexpr static double pointSelectRadius = 20.0;

    bool m_drawing{ false };

    Ui::MainWindow *m_ui;
    QLabel *m_mouseCoordinatesIcon;
    QLabel *m_mouseCoordinatesLabel;
    QLabel *m_screenSizeIcon;
    QLabel *m_screenSizeLabel;
    PresaveDialog *m_saveDialog;
    ComparisonDisplay *m_compDisplay;

    QMediaPlayer *m_player{ nullptr };
    QGraphicsVideoItem *m_video{ nullptr };

    QImage m_image;
    cv::Mat m_imageGray;
    std::unique_ptr<DiagonalGraph<CostFunction>> m_graph{ nullptr };

    // TODO: add selection color dialog
    PainterOptions m_painterOptions{ {}, Qt::red, Qt::red, QColor(255, 160, 160, 70), Qt::white, 1, 4 };

    QRectF m_initialSceneRect{};
    QGraphicsScene *m_scene;
    QPoint m_mouseLastScenePosition{};
    PathItem *m_path{ nullptr };
    SelectionLayerItem *m_selectionItem{ nullptr };
    QUndoStack *m_undoStack;
    PointItem *m_selectedPoint{ nullptr };
    int m_numberOfPoints;
    int m_numberOfScheduledScalings;
};

#endif  // MAIN_WINDOW_HPP
