#include "paint_area.hpp"

#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QMouseEvent>
#include <QPainter>
#include <numeric>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <queue>

#include "cost_functions.hpp"
#include "dijkstra.hpp"
#include "io.hpp"

PaintArea::PaintArea(QWidget *parent) : QWidget{ parent } {}

void PaintArea::open(const QString &filePath) {
    QImageReader reader(filePath);
    image = reader.read();
    scaleRatio = 1.f;
    editing = false;
    movingPointId = -1;
    setMinimumSize(image.size());
    setMouseTracking(true);
    clear();

    cv::Mat temp = cv::imread(filePath.toStdString());
    cv::cvtColor(temp, imageGray, cv::COLOR_BGR2GRAY);
    graph = DistanceGraph(imageGray, improvedCostFunc);
    region = std::vector<bool>(image.width() * image.height(), false);
}

void PaintArea::loadOutlines(const QString &filePath) {
    // load whole path as one edge
    Path loadedPath;
    loadedPath.points =
        readPointsInto<QList<QPoint>>(filePath.toStdString().c_str());
    loadedPath.penColor = pens.currentPath.color();
    loadedPath.penWidth = pens.currentPath.width();
    lastPoint = loadedPath.points.last();
    points.clear();
    points.push_back(loadedPath.points.first());
    points.push_back(loadedPath.points.last());
    fullPath.push_back(loadedPath);

    update();
}

void PaintArea::loadOutlinesWithColors(const QString &filePath) {
    points.clear();
    fullPath.clear();

    std::ifstream file(filePath.toStdString());
    // number of edges
    int size;
    file >> size;

    points.reserve(size + 1);

    // load multiple edges
    for (int i = 0; i < size; ++i) {
        Path loadedPath;
        file >> loadedPath.penWidth;

        int r, g, b;
        file >> r >> g >> b;
        loadedPath.penColor = QColor::fromRgb(r, g, b);

        int pathSize;
        file >> pathSize;

        loadedPath.points.reserve(pathSize);
        for (int j = 0; j < pathSize; ++j) {
            int x, y;
            file >> x >> y;
            loadedPath.points.push_back({ x, y });
        }
        // push back first point of the edge
        points.push_back(loadedPath.points.first());
        fullPath.push_back(loadedPath);
    }
    // complete list of points with last point of last edge
    points.push_back(fullPath.last().points.last());
    lastPoint = points.last();

    update();
}

void PaintArea::saveOutlines(const QString &filePath) {
    std::ofstream outFile(filePath.toStdString(), std::ios::out);
    for (const auto &pathPart : fullPath)
        for (const auto &point : pathPart.points) {
            outFile << point.x() << ' ' << point.y() << '\n';
        }

    const auto baseName = QFileInfo(filePath).baseName();
    const auto extension = QFileInfo(filePath).completeSuffix();
    const auto path = QFileInfo(filePath).absolutePath();
    const auto fileName = path + "/" + baseName + "_c." + extension;
    outFile.close();
    outFile.open(fileName.toStdString(), std::ios::out);

    // saving number of paths
    outFile << fullPath.size() << '\n';

    for (const auto &pathPart : fullPath) {
        outFile << pathPart.penWidth << '\n';
        outFile << pathPart.penColor.red() << ' ' << pathPart.penColor.green()
                << ' ' << pathPart.penColor.blue() << '\n';
        outFile << pathPart.points.size() << '\n';
        for (const auto &point : pathPart.points)
            outFile << point.x() << ' ' << point.y() << '\n';
    }
    outFile << '\n';
}

void PaintArea::save(const QString &filePath, SaveOptions opts) {
    QImageWriter writer(filePath);
    writer.write(composeImage(opts));
}

void PaintArea::setDrawingMode() { editing = false; }

void PaintArea::setEditingMode() {
    // lastEdge.clear();
    editing = true;
}

void PaintArea::undo() {
    if (!points.isEmpty()) {
        points.removeLast();
        if (points.isEmpty()) {
            lastPoint = {};
            lastEdge.clear();
        } else {
            fullPath.removeLast();
            lastPoint = points.last();
            const auto currentPoint = QWidget::mapFromGlobal(QCursor::pos());
            const auto path = dijkstra(graph, { lastPoint.x(), lastPoint.y() },
                                       { currentPoint.x(), currentPoint.y() });
            setLastEdge(path);
        }
    }
    update();
}

void PaintArea::zoomIn() {
    if (scaleRatio > 8.f) return;
    scaleRatio *= 2;
    setMinimumSize(image.size() * scaleRatio);
    update();
}

void PaintArea::zoomOut() {
    if (scaleRatio < 1.f / 8.f) return;
    scaleRatio /= 2;
    setMinimumSize(image.size() * scaleRatio);
    update();
}

void PaintArea::setPenWidth(int width) { pens.currentEdge.setWidth(width); }

void PaintArea::setPenColor(const QColor &color) {
    pens.currentEdge.setColor(color);
}

void PaintArea::finalizePath() {
    if (points.size() <= 1) return;

    const auto finalEdge = dijkstra(graph, Point::from(points.back()),
                                    Point::from(points.front()));
    setLastEdge(finalEdge);

    if (!lastEdge.empty()) {
        fullPath.push_back(
            Path(lastEdge, pens.currentEdge.width(), pens.currentEdge.color()));
        // overlap last and first points
        points.push_back(lastEdge.last());
    }

    lastEdge.clear();
    lastPoint = {};

    update();
}

QList<QPoint> PaintArea::createPath(QPoint pointFrom, QPoint pointTo) {
    QList<QPoint> edge;
    const auto dijkstraPath =
        dijkstra(graph, Point::from(pointFrom), Point::from(pointTo));
    edge.clear();
    edge.reserve(dijkstraPath.size());
    for (const auto point : dijkstraPath) {
        edge.push_back(
            { static_cast<int>(point.x), static_cast<int>(point.y) });
    }
    return edge;
}

void PaintArea::updatePathsAfterMoving(int pointId) {
    if (pointId == 0) {
        // updating first edge
        fullPath.first().points =
            createPath(points.at(pointId), points.at(pointId + 1));
    } else if (pointId == points.size() - 1) {
        // updating last edge
        fullPath.last().points =
            createPath(points.at(pointId - 1), points.at(pointId));
    } else {
        // updating both edges connected to middle point
        Path edge = fullPath.at(pointId);
        edge.points = createPath(points.at(pointId), points.at(pointId + 1));
        fullPath.replace(pointId, edge);
        edge = fullPath.at(pointId - 1);
        edge.points = createPath(points.at(pointId - 1), points.at(pointId));
        fullPath.replace(pointId - 1, edge);
    }
}

void PaintArea::updatePathsAfterDeleting(int pointId) {
    if (points.size() <= 1) {
        if (points.size() == 1)
            lastPoint = points.front();
        else
            lastPoint = {};
        fullPath.clear();
        return;
    }
    if (pointId == 0) {
        fullPath.removeFirst();
        return;
    } else if (pointId == points.size()) {
        fullPath.removeLast();
        lastPoint = points.last();
        return;
    }
    int edgeId = pointId - 1;
    fullPath.remove(edgeId);
    const auto dijkstraPath =
        dijkstra(graph, Point::from(points.at(pointId - 1)),
                 Point::from(points.at(pointId)));
    Path modifiedPath;
    modifiedPath.penWidth = fullPath.at(edgeId).penWidth;
    modifiedPath.penColor = fullPath.at(edgeId).penColor;
    modifiedPath.points.reserve(dijkstraPath.size());
    for (const auto point : dijkstraPath) {
        modifiedPath.points.push_back(
            { static_cast<int>(point.x), static_cast<int>(point.y) });
    }
    fullPath.replace(edgeId, modifiedPath);
}

void PaintArea::movePoint(const QPoint mPos) {
    if (movingPointId == -1) {
        // unselected point - find point at mouse position
        int pointId = 0;
        for (auto it = points.begin(); it != points.end(); it++) {
            if (std::abs(it->x() - mPos.x()) < 10 &&
                std::abs(it->y() - mPos.y()) < 10) {
                movingPointId = pointId;
                return;
            }
            pointId++;
        }
    } else if (editing && movingPointId >= 0) {
        // selected point - move at mouse position
        points.replace(movingPointId, mPos);
        updatePathsAfterMoving(movingPointId);
        if (movingPointId == points.size() - 1) lastPoint = points.last();
        movingPointId = -1;
    }
}

void PaintArea::deletePoint(const QPoint mPos) {
    // find and erase point at mouse position
    int pointId = 0;
    for (auto it = points.begin(); it != points.end(); it++) {
        if (std::abs(it->x() - mPos.x() / scaleRatio) < 10 &&
            std::abs(it->y() - mPos.y() / scaleRatio) < 10) {
            points.erase(it);
            updatePathsAfterDeleting(pointId);
            break;
        }
        pointId++;
    }
}

void PaintArea::mousePressEvent(QMouseEvent *event) {
    if (editing) {
        const auto currentPoint = event->position().toPoint() / scaleRatio;
        if (event->buttons() & Qt::LeftButton)
            movePoint(currentPoint);
        else if (event->buttons() & Qt::RightButton)
            deletePoint(currentPoint);
    } else if (event->buttons() & Qt::LeftButton) {
        const auto currentPoint = event->position().toPoint() / scaleRatio;
        lastPoint = currentPoint;
        points.push_back(lastPoint);
        if (!lastEdge.empty()) {
            Path pathToAdd(lastEdge, pens.currentEdge.width(),
                           pens.currentEdge.color());
            fullPath.push_back(pathToAdd);
        }
    } else if (event->buttons() & Qt::RightButton) {
        const auto pd = getConfirmedPoints();
        if (!pd.pts.empty()) {
            const auto p = event->position().toPoint() / scaleRatio;
            floodFillRegion(pd, { p.x(), p.y() });
        }
    }
    update();
}

void PaintArea::mouseMoveEvent(QMouseEvent *event) {
    if (lastPoint.isNull() || editing) return;

    const auto currentPoint = event->position().toPoint() / scaleRatio;
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

    painter.scale(scaleRatio, scaleRatio);
    dirtyRect.setTopLeft(QPoint(0, 0));

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
    fullPath.clear();
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
    for (const auto &pathPart : fullPath)
        for (const auto &point : pathPart.points) {
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
        for (const auto &pathPart : fullPath) {
            pens.currentPath.setWidth(pathPart.penWidth);
            pens.currentPath.setColor(pathPart.penColor);
            painter.setPen(pens.currentPath);
            painter.drawPoints(pathPart.points);
        }
    }

    if (paintPoints) {
        painter.setPen(pens.point);
        painter.drawPoints(points);
    }

    if (paintCurrent && !editing) {
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

QImage PaintArea::composeImage(SaveOptions opts) {
    static constexpr QColor empty{ 0, 0, 0, 0 };
    QImage resultImage = image;
    QColor outFillColor;

    if (opts.binarize) {
        outFillColor = Qt::black;
        resultImage.fill(Qt::white);
    } else {
        outFillColor = empty;
    }

    if (opts.saveInside != opts.saveOutside) {
        for (int y = 0; y < image.height(); ++y)
            for (int x = 0; x < image.width(); ++x)
                if (region[image.width() * y + x] == opts.saveOutside)
                    resultImage.setPixelColor(x, y, outFillColor);
    } else if (!opts.saveInside) {
        resultImage.fill(outFillColor);
    }

    if (opts.savePath || opts.savePoints) {
        QPainter painter(&resultImage);
        paintPathComponents(painter, false, opts.savePath, opts.savePoints);
    }

    return resultImage;
}
