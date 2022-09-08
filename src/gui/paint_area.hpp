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
    void undo();

    void setPenWidth(int width);
    void setPenColor(const QColor &color);

    void finalizePath();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    bool drawing = false;
    DistanceGraph graph;
    cv::Mat imageGray;
    QImage image;
    QPoint lastPoint;
    QList<QPoint> lastEdge;
    QList<QPoint> points;
    QList<QPoint> currentPath;
    QList<QList<QPoint>> previousPaths;
    QList<int> pathsLengths;
    QList<int> pathsWidths;
    QList<QColor> pathsColors;
    // QList<QPoint> region;
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
