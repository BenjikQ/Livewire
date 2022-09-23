#include "path_item.hpp"

#include <algorithm>

#include <QBrush>
#include <QPainter>
#include <QPen>

#include "painter_options.hpp"

PathItem::PathItem(const PainterOptions &options, const QList<QPoint> &points, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    m_points{ points },
    m_color{ options.pathColor } {
    const auto [minX, maxX] = std::minmax_element(m_points.cbegin(), m_points.cend(),
                                                  [](const auto &lhs, const auto &rhs) { return lhs.x() < rhs.x(); });
    const auto [minY, maxY] = std::minmax_element(m_points.cbegin(), m_points.cend(),
                                                  [](const auto &lhs, const auto &rhs) { return lhs.y() < rhs.y(); });
    const QPointF topLeft{ static_cast<double>(minX->x()), static_cast<double>(minY->y()) };
    const QPointF bottomRight{ static_cast<double>(maxX->x()), static_cast<double>(maxY->y()) };
    m_boundRect = QRectF{ topLeft, bottomRight };
}

void PathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    const QPen pen{ m_color, 4 };
    painter->setPen(pen);
    painter->drawPoints(m_points);
}

QRectF PathItem::boundingRect() const {
    return m_boundRect;
}

void PathItem::setPoints(QList<QPoint> points) {
    m_points = std::move(points);
}

QList<QPoint> PathItem::getPoints() const {
    return m_points;
}

void PathItem::setOptions(const PainterOptions &options) {
    m_color = options.pathColor;
}
