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

struct PainterOptions;

class PathItem : public QGraphicsItem {
public:
    int number;

    enum { Type = UserType + 1 };

    explicit PathItem(int pathNo, const PainterOptions &options, const QList<QPoint> &points = {},
                      QGraphicsItem *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    [[nodiscard]] QRectF boundingRect() const override;
    [[nodiscard]] int type() const override;

    void setPoints(QList<QPoint> points);
    QList<QPoint> getPoints() const;
    void setOptions(const PainterOptions &options);

private:
    void updateBoundRect();

private:
    QList<QPoint> m_points;
    QColor m_color;
    mutable QRectF m_boundRect;
};

#endif  // LIVEWIRE_PATH_ITEM_HPP
