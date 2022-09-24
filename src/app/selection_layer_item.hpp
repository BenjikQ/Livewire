#ifndef SELECTIONLAYERITEM_HPP
#define SELECTIONLAYERITEM_HPP

#include <vector>

#include <QGraphicsItem>
#include <QRectF>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
QT_BEGIN_NAMESPACE

struct PainterOptions;

class SelectionLayerItem : public QGraphicsItem {
public:
    explicit SelectionLayerItem(const PainterOptions &options, QGraphicsItem *parent = nullptr);
    explicit SelectionLayerItem(const PainterOptions &options, int w, int h, QGraphicsItem *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    [[nodiscard]] QRectF boundingRect() const override;

    inline int w() const { return m_w; }
    inline int h() const { return m_h; }
    const std::vector<bool> &selected() const;

    void setSize(int w, int h);
    void setSelected(const std::vector<bool> &sel);
    void flipSelected(const std::vector<bool> &difference);

private:
    QColor m_color;
    std::vector<bool> m_selectedPixels;
    int m_w = 0, m_h = 0;
};

#endif  // SELECTIONLAYERITEM_HPP
