#include "paint_area.hpp"

#include <QImageReader>
#include <QImageWriter>
#include <QMouseEvent>
#include <QPainter>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "cost_functions.hpp"
#include "dijkstra.hpp"

PaintArea::PaintArea(QWidget *parent) : QWidget{ parent } {}

void PaintArea::open(const QString &filePath) {
    QImageReader reader(filePath);
    image = reader.read();
    setMinimumSize(image.size());
    setMouseTracking(true);

    cv::Mat temp = cv::imread(filePath.toStdString());
    cv::cvtColor(temp, imageGray, cv::COLOR_BGR2GRAY);
    graph = DistanceGraph(imageGray, improvedCostFunc);
}

void PaintArea::save(const QString &filePath) {
    QImageWriter writer(filePath);
    writer.write(image);
}

void PaintArea::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        const auto currentPoint = event->position().toPoint();
        const QColor penColor = Qt::red;
        const int penWidth = 10;
        QPainter painter(&image);
        painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        lines.push_back({ lastPoint, currentPoint });
        lastPoint = currentPoint;
        points.push_back(lastPoint);
        update();
    }
}

void PaintArea::mouseMoveEvent(QMouseEvent *event) {
    if (lastPoint.isNull()) return;

    const auto currentPoint = event->position().toPoint();
    if (currentPoint.x() >= image.width() || currentPoint.y() >= image.height())
        return;

    const auto path = dijkstra(graph, { lastPoint.x(), lastPoint.y() },
                               { currentPoint.x(), currentPoint.y() });
    lastEdge.clear();
    lastEdge.reserve(path.size());
    for (const auto point : path) {
        lastEdge.push_back(
            { static_cast<int>(point.x), static_cast<int>(point.y) });
    }
    update();
}

void PaintArea::paintEvent(QPaintEvent *event) {
    QColor penColor = Qt::red;
    int penWidth = 10;
    QPainter painter(this);
    painter.setPen(
        QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QRect dirtyRect = event->rect();
    if (lines.size() == 1) {
        lines[0] = { lines[0].p2(), lines[0].p2() };
    }
    painter.drawImage(dirtyRect, image, dirtyRect);
    painter.drawPoints(points);

    penWidth = 2;
    penColor = Qt::blue;
    painter.setPen(
        QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLines(lines);
    penWidth = 3;
    penColor = Qt::green;
    painter.setPen(
        QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPoints(lastEdge);
}
