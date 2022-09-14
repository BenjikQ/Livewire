#ifndef LIVEWIRE_PATH_ITEM_HPP
#define LIVEWIRE_PATH_ITEM_HPP

#include <QGraphicsItem>
#include <QList>
#include <QPoint>
#include <QRectF>

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

class PathItem : public QGraphicsItem {
public:
    explicit PathItem(const QList<QPoint> &points = {}, QGraphicsItem *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    [[nodiscard]] QRectF boundingRect() const override;

    void setPoints(QList<QPoint> points);

private:
    QList<QPoint> m_points;
};

#endif  // LIVEWIRE_PATH_ITEM_HPP
