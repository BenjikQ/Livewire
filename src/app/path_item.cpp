#include "path_item.hpp"

#include <algorithm>

#include <QBrush>
#include <QPainter>
#include <QPen>

#include "painter_options.hpp"

PathItem::PathItem(const PainterOptions &options, const QList<QPoint> &points, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    m_points{ points },
    m_color{ options.pathColor } {}

void PathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    const QPen pen{ m_color, 4 };
    painter->setPen(pen);
    painter->drawPoints(m_points);
}

QRectF PathItem::boundingRect() const {
    const auto minMaxX = std::minmax(m_points.cbegin(), m_points.cend(),
                                     [](const auto &lhs, const auto &rhs) { return lhs->x() < rhs->x(); });
    const auto minMaxY = std::minmax(m_points.cbegin(), m_points.cend(),
                                     [](const auto &lhs, const auto &rhs) { return lhs->y() < rhs->y(); });

    const QPointF topLeft{ static_cast<double>(minMaxX.first->x()), static_cast<double>(minMaxY.first->y()) };
    const QPointF bottomRight{ static_cast<double>(minMaxX.second->x()), static_cast<double>(minMaxY.second->y()) };

    return QRectF{ topLeft, bottomRight };
}

void PathItem::setPoints(QList<QPoint> points) {
    m_points = std::move(points);
}

void PathItem::setOptions(const PainterOptions &options) {
    m_color = options.pathColor;
}
