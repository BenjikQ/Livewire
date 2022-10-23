#ifndef LIVEWIRE_POINT_ITEM_HPP
#define LIVEWIRE_POINT_ITEM_HPP

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QPen>
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
    int number;
    bool selected{ false };

    enum { Type = UserType + 2 };

    explicit PointItem(int pointNo, const PainterOptions &options, QGraphicsItem *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    [[nodiscard]] QRectF boundingRect() const override;
    [[nodiscard]] QPainterPath shape() const override;
    [[nodiscard]] int type() const override;

private:
    QPointF m_position;
    QPen m_selectionPen;
    QBrush m_selectionBrush;
    float m_radius;
    float m_width;
};

#endif  // LIVEWIRE_POINT_ITEM_HPP
