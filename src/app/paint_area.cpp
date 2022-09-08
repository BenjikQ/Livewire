#include "paint_area.hpp"

#include <QImageReader>
#include <QPaintEvent>
#include <QPainter>
#include <QRect>

PaintArea::PaintArea(QWidget *parent) :
    QWidget{ parent } {}

void PaintArea::open(const QString &filePath) {
    QImageReader reader{ filePath };
    m_image = reader.read();
    setMinimumSize(m_image.size());
    setMouseTracking(true);
}

void PaintArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, m_image, dirtyRect);
}
