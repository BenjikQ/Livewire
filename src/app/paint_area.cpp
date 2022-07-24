#include "paint_area.hpp"

#include <QImageReader>
#include <QImageWriter>
#include <QMouseEvent>
#include <QPainter>

PaintArea::PaintArea(QWidget *parent) : QWidget{ parent } {}

void PaintArea::open(const QString &filePath) {
    QImageReader reader(filePath);
    image = reader.read();
    setMinimumSize(image.size());
}

void PaintArea::save(const QString &filePath) {
    QImageWriter writer(filePath);
    writer.write(image);
}

void PaintArea::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->position().toPoint();
        drawing = true;
    }
}

void PaintArea::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && drawing)
        drawLineTo(event->position().toPoint());
}

void PaintArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && drawing) {
        drawLineTo(event->position().toPoint());
        drawing = false;
    }
}

void PaintArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}

void PaintArea::resizeEvent(QResizeEvent *event) {}

void PaintArea::drawLineTo(const QPoint &endPoint) {
    const QColor penColor = Qt::blue;
    const int penWidth = 1;
    QPainter painter(&image);
    painter.setPen(
        QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);
    int rad = (penWidth / 2) + 2;
    update(QRect(lastPoint, endPoint)
               .normalized()
               .adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}
