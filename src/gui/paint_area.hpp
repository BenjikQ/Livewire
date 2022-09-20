#ifndef PAINTAREA_HPP
#define PAINTAREA_HPP

#include <QList>
#include <QPen>
#include <QWidget>
#include <opencv2/core.hpp>

#include "image_graph.hpp"

class PaintArea : public QWidget {
    Q_OBJECT
public:
    explicit PaintArea(QWidget *parent = nullptr);

    void open(const QString &filePath);
    void save(const QString &filePath, SaveOptions opts);
    void loadOutlines(const QString &filePath);
    void loadOutlinesWithColors(const QString &filePath);
    void saveOutlines(const QString &filePath);
    void undo();
    void clear();  // clearing PaintArea available from ImageViewer
    void zoomIn();
    void zoomOut();
    void setDrawingMode();
    void setEditingMode();

    void setPenWidth(int width);
    void setPenColor(const QColor &color);

    void finalizePath();
    QImage composeImage(SaveOptions opts);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QList<QPoint> createPath(QPoint pointFrom, QPoint pointTo);
    void movePoint(const QPoint mPos);
    void updatePathsAfterMoving(int pointId);
    void deletePoint(const QPoint mPos);
    void updatePathsAfterDeleting(int pointId);

private:
    bool editing;
    int movingPointId;
    float scaleRatio;
    DistanceGraph graph;
    cv::Mat imageGray;
    QImage image;

    class Path {
    public:
        Path() {}
        Path(QList<QPoint> p, int w, QColor c) :
            points(p), penWidth(w), penColor(c) {}
        QList<QPoint> points;
        int penWidth;
        QColor penColor;
    };
    QList<Path> fullPath;
    QList<QPoint> points;
    QList<QPoint> lastEdge;
    QPoint lastPoint;

    std::vector<bool> region;

    struct {
        QPen point{ Qt::red, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin };
        QPen currentEdge{ Qt::green, 3, Qt::SolidLine, Qt::RoundCap,
                          Qt::RoundJoin };
        QPen currentPath{ Qt::blue, 2, Qt::SolidLine, Qt::RoundCap,
                          Qt::RoundJoin };
        QPen previousPath{ Qt::magenta, 2, Qt::SolidLine, Qt::RoundCap,
                           Qt::RoundJoin };
        QPen region{ QColor(255, 90, 255, 50), 1, Qt::SolidLine, Qt::RoundCap,
                     Qt::RoundJoin };
    } pens;

    void setLastEdge(const std::vector<Point> &path);
    PathData getConfirmedPoints() const;
    void floodFillRegion(const PathData &pd, Point origin);
    void paintPathComponents(QPainter &painter, bool paintCurrent = true,
                             bool paintPath = true, bool paintPoints = true);
};

#endif  // PAINTAREA_HPP
