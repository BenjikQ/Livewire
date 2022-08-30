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

void PaintArea::finalizePath() {
    if (points.size() <= 1) return;

    const auto finalEdge = dijkstra(graph, Point::from(points.front()),
                                    Point::from(points.back()));
    setLastEdge(finalEdge);
    std::copy(lastEdge.cbegin(), lastEdge.cend(),
              std::back_inserter(currentPath));

    previousPaths.push_back(currentPath);
    currentPath.clear();
    lastEdge.clear();
    points.clear();
    lastPoint = {};

    update();
}

void PaintArea::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        const auto currentPoint = event->position().toPoint();
        const QColor penColor = Qt::red;
        const int penWidth = 10;
        QPainter painter(&image);
        painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        // lines.push_back({ lastPoint, currentPoint });
        lastPoint = currentPoint;
        std::copy(lastEdge.cbegin(), lastEdge.cend(),
                  std::back_inserter(currentPath));
        points.push_back(lastPoint);
    } else if (event->buttons() & Qt::RightButton) {
    }
    update();
}

void PaintArea::mouseMoveEvent(QMouseEvent *event) {
    if (lastPoint.isNull()) return;

    const auto currentPoint = event->position().toPoint();
    if (currentPoint.x() >= image.width() || currentPoint.y() >= image.height())
        return;

    const auto path = dijkstra(graph, { lastPoint.x(), lastPoint.y() },
                               { currentPoint.x(), currentPoint.y() });
    setLastEdge(path);
    update();
}

void PaintArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QRect dirtyRect = event->rect();

    painter.drawImage(dirtyRect, image, dirtyRect);

    painter.setPen(pens.point);
    painter.drawPoints(points);

    painter.setPen(pens.currentPath);
    painter.drawPoints(currentPath);

    painter.setPen(pens.currentEdge);
    painter.drawPoints(lastEdge);

    painter.setPen(pens.previousPath);
    for (const auto &points : previousPaths)
        painter.drawPoints(points);
}

void PaintArea::setLastEdge(const std::vector<Point> &path) {
    lastEdge.clear();
    lastEdge.reserve(path.size());
    for (const auto point : path) {
        lastEdge.push_back(
            { static_cast<int>(point.x), static_cast<int>(point.y) });
    }
}
