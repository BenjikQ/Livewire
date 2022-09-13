#ifndef LIVEWIRE_POINT_ITEM_HPP
#define LIVEWIRE_POINT_ITEM_HPP

#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QRectF>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
QT_BEGIN_NAMESPACE

struct PainterOptions;

class PointItem : public QGraphicsEllipseItem {
public:
    explicit PointItem(const PainterOptions &options, QGraphicsItem *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    [[nodiscard]] QRectF boundingRect() const override;

private:
    QPointF m_position;
    float m_radius;
    float m_width;
};

#endif  // LIVEWIRE_POINT_ITEM_HPP
