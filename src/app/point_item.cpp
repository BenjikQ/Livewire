#include "point_item.hpp"

#include <QBrush>
#include <QGraphicsItem>
#include <QPainter>
#include <QPen>

#include "painter_options.hpp"

PointItem::PointItem(int pointNo, const PainterOptions &options, QGraphicsItem *parent) :
    QGraphicsEllipseItem{ parent },
    m_position{ options.position },
    m_radius{ options.radius },
    m_width{ options.width },
    number{ pointNo } {
    const QBrush inner{ options.pointColor };
    const QBrush outer{ options.pointColor };
    const QPen pen{ outer, m_width };
    m_color = outer.color();
    setPen(pen);
    setBrush(inner);
    setPos(m_position);

    const QBrush innerS{ options.pointSelectionColor };
    const QBrush outerS{ options.pointSelectionColor };
    const QPen penS{ outerS, m_width + 3 };
    m_selectionPen = penS;
    m_selectionBrush = innerS;

    setZValue(1);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
}

void PointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (selected) {
        painter->setPen(m_selectionPen);
        painter->setBrush(m_selectionBrush);
    } else {
        painter->setPen(pen());
        painter->setBrush(brush());
    }
    painter->drawEllipse({ 0, 0 }, m_radius, m_radius);
}

QRectF PointItem::boundingRect() const {
    return { -m_radius - m_width / 2, -m_radius - m_width / 2, 2 * m_radius + m_width, 2 * m_radius + m_width };
}

QPainterPath PointItem::shape() const {
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

int PointItem::type() const {
    return Type;
}
