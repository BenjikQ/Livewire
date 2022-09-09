#include "paint_area.hpp"

#include <QImageReader>
#include <QImageWriter>
#include <QMouseEvent>
#include <QPainter>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <queue>

#include "cost_functions.hpp"
#include "dijkstra.hpp"

PaintArea::PaintArea(QWidget *parent) : QWidget{ parent } {}

void PaintArea::open(const QString &filePath) {
    QImageReader reader(filePath);
    image = reader.read();
    setMinimumSize(image.size());
    setMouseTracking(true);
    clear();

    cv::Mat temp = cv::imread(filePath.toStdString());
    cv::cvtColor(temp, imageGray, cv::COLOR_BGR2GRAY);
    graph = DistanceGraph(imageGray, improvedCostFunc);
    region = std::vector<bool>(image.width() * image.height(), false);
}

void PaintArea::save(const QString &filePath, SaveOptions opts) {
    static const QColor empty{ 0, 0, 0, 0 };
    QImageWriter writer(filePath);
    QImage resultImage = image;

    if (opts.saveInside != opts.saveOutside) {
        for (int y = 0; y < image.height(); ++y)
            for (int x = 0; x < image.width(); ++x)
                if (region[image.width() * y + x] == opts.saveOutside)
                    resultImage.setPixelColor(x, y, empty);
    } else if (!opts.saveInside) {
        resultImage.fill(empty);
    }

    if (opts.savePath || opts.savePoints) {
        QPainter painter(&resultImage);
        paintPathComponents(painter, false, opts.savePath, opts.savePoints);
    }

    writer.write(resultImage);
}

void PaintArea::undo() {
    if (!points.isEmpty()) {
        points.removeLast();
        if (points.isEmpty()) {
            lastPoint = {};
            lastEdge.clear();
        } else {
            // Remove last saved path
            const auto len = pathsLengths.last();
            const auto index = currentPath.size() - len;
            currentPath.remove(index, len);
            pathsLengths.removeLast();
            pathsWidths.removeLast();
            pathsColors.removeLast();

            lastPoint = points.last();
            const auto currentPoint = QWidget::mapFromGlobal(QCursor::pos());
            const auto path = dijkstra(graph, { lastPoint.x(), lastPoint.y() },
                                       { currentPoint.x(), currentPoint.y() });
            setLastEdge(path);
        }
    }

    update();
}

void PaintArea::setPenWidth(int width) { pens.currentEdge.setWidth(width); }

void PaintArea::setPenColor(const QColor &color) {
    pens.currentEdge.setColor(color);
}

void PaintArea::finalizePath() {
    if (points.size() <= 1) return;

    const auto finalEdge = dijkstra(graph, Point::from(points.front()),
                                    Point::from(points.back()));
    setLastEdge(finalEdge);

    // TODO: maybe fix copypaste
    currentPath.append(lastEdge.cbegin(), lastEdge.cend());
    if (!lastEdge.empty()) {
        pathsLengths.push_back(lastEdge.size());
        pathsWidths.push_back(pens.currentEdge.width());
        pathsColors.push_back(pens.currentEdge.color());
    }

    lastEdge.clear();
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
        lastPoint = currentPoint;
        currentPath.append(lastEdge.cbegin(), lastEdge.cend());
        if (!lastEdge.empty()) {
            pathsLengths.push_back(lastEdge.size());
            pathsWidths.push_back(pens.currentEdge.width());
            pathsColors.push_back(pens.currentEdge.color());
        }
        points.push_back(lastPoint);
    } else if (event->buttons() & Qt::RightButton) {
        const auto pd = getConfirmedPoints();
        if (!pd.pts.empty()) {
            const auto p = event->position().toPoint();
            floodFillRegion(pd, { p.x(), p.y() });
        }
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
    paintPathComponents(painter);

    //    painter.setPen(pens.point);
    //    painter.drawPoints(points);

    //    //    painter.setPen(pens.currentPath);
    //    //    painter.drawPoints(currentPath);

    //    int start = 0;
    //    for (int i = 0; i < pathsWidths.size(); ++i) {
    //        const auto width = pathsWidths[i];
    //        const auto length = pathsLengths[i];
    //        const auto color = pathsColors[i];
    //        const auto &path =
    //            QList<QPoint>{ currentPath.begin() + start,
    //                           currentPath.begin() + start + length };
    //        pens.currentPath.setWidth(width);
    //        pens.currentPath.setColor(color);
    //        painter.setPen(pens.currentPath);
    //        painter.drawPoints(path);
    //        start += length;
    //    }

    //    painter.setPen(pens.currentEdge);
    //    painter.drawPoints(lastEdge);

    //    //    painter.setPen(pens.previousPath);
    //    //    for (const auto &points : previousPaths)
    //    //        painter.drawPoints(points);

    //    painter.setPen(pens.region);
    //    for (int x = 0; x < image.width(); ++x) {
    //        for (int y = 0; y < image.height(); ++y) {
    //            if (region[y * image.width() + x]) {
    //                painter.drawPoint(QPoint{ x, y });
    //            }
    //        }
    //    }
}

void PaintArea::clear() {
    lastPoint = {};
    lastEdge.clear();
    points.clear();
    currentPath.clear();
    previousPaths.clear();
    pathsLengths.clear();
    pathsWidths.clear();
    pathsColors.clear();
    region.clear();
}

void PaintArea::setLastEdge(const std::vector<Point> &path) {
    lastEdge.clear();
    lastEdge.reserve(path.size());
    for (const auto point : path) {
        lastEdge.push_back(
            { static_cast<int>(point.x), static_cast<int>(point.y) });
    }
}

PathData PaintArea::getConfirmedPoints() const {
    PathData data{ Point::maxValue(), -Point::maxValue(), {} };

    // for (const auto &path : previousPaths) {
    for (const auto &point : currentPath) {
        data.topLeft.x =
            std::min(data.topLeft.x, static_cast<int64_t>(point.x()));
        data.topLeft.y =
            std::min(data.topLeft.y, static_cast<int64_t>(point.y()));
        data.bottomRight.x =
            std::max(data.bottomRight.x, static_cast<int64_t>(point.x()));
        data.bottomRight.y =
            std::max(data.bottomRight.y, static_cast<int64_t>(point.y()));
        data.pts.insert(Point::from(point));
    }
    //}

    return data;
}

void PaintArea::floodFillRegion(const PathData &pd, Point origin) {
    const auto w = image.width();
    const auto h = image.height();
    const auto index = [=](const Point &p) -> int64_t { return p.y * w + p.x; };
    if (index(origin) >= region.size()) return;
    const bool fillValue = !region[index(origin)];

    Point cur, adj;
    std::queue<Point> to_visit{};
    to_visit.push(origin);

    while (!to_visit.empty()) {
        cur = to_visit.front();
        to_visit.pop();

        for (Dir d{}; d < Dir::COUNT; d += 2) {
            adj = cur + d;
            if (adj.x >= 0 && adj.y >= 0 && adj.x < w && adj.y < h &&
                region[index(adj)] != fillValue && !pd.pts.contains(adj)) {
                to_visit.push(adj);
                region[index(adj)] = fillValue;
            }
        }
    }
}

void PaintArea::paintPathComponents(QPainter &painter, bool paintCurrent,
                                    bool paintPath, bool paintPoints) {
    if (paintPath) {
        int start = 0;
        for (int i = 0; i < pathsWidths.size(); ++i) {
            const auto width = pathsWidths[i];
            const auto length = pathsLengths[i];
            const auto color = pathsColors[i];
            const auto &path =
                QList<QPoint>{ currentPath.begin() + start,
                               currentPath.begin() + start + length };
            pens.currentPath.setWidth(width);
            pens.currentPath.setColor(color);
            painter.setPen(pens.currentPath);
            painter.drawPoints(path);
            start += length;
        }
    }

    if (paintPoints) {
        painter.setPen(pens.point);
        painter.drawPoints(points);
    }

    if (paintCurrent) {
        painter.setPen(pens.currentEdge);
        painter.drawPoints(lastEdge);

        painter.setPen(pens.region);
        for (int x = 0; x < image.width(); ++x) {
            for (int y = 0; y < image.height(); ++y) {
                if (region[y * image.width() + x]) {
                    painter.drawPoint(QPoint{ x, y });
                }
            }
        }
    }
}
