#ifndef PAINTAREA_HPP
#define PAINTAREA_HPP

#include <QList>
#include <QWidget>
#include <opencv2/core.hpp>

#include "image_graph.hpp"

class PaintArea : public QWidget {
    Q_OBJECT
public:
    explicit PaintArea(QWidget *parent = nullptr);

    void open(const QString &filePath);
    void save(const QString &filePath);

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
    QList<QPoint> fullPath;
    // QList<QLine> lines;
};

#endif  // PAINTAREA_HPP
