#include "point_item.hpp"

#include <QBrush>
#include <QGraphicsItem>
#include <QPainter>
#include <QPen>

#include "painter_options.hpp"

PointItem::PointItem(const PainterOptions &options, QGraphicsItem *parent) :
    QGraphicsEllipseItem{ parent },
    m_position{ options.position },
    m_radius{ options.radius },
    m_width{ options.width } {
    const QBrush inner{ options.pointColor };
    const QBrush outer{ options.pointColor };
    const QPen pen{ outer, m_width };
    setPen(pen);
    setBrush(inner);
    setPos(m_position);
}

void PointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawEllipse({ 0, 0 }, m_radius, m_radius);
}

QRectF PointItem::boundingRect() const {
    return { -m_radius - m_width / 2, -m_radius - m_width / 2, 2 * m_radius + m_width, 2 * m_radius + m_width };
}

int PointItem::type() const {
    return Type;
}
