#include "selection_layer_item.hpp"

#include <algorithm>

#include <QPainter>
#include <QPen>

#include "painter_options.hpp"

SelectionLayerItem::SelectionLayerItem(const PainterOptions &options, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    m_color(options.selectionColor) {}

SelectionLayerItem::SelectionLayerItem(const PainterOptions &options, int w, int h, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    m_color(options.selectionColor),
    m_w(w),
    m_h(h),
    m_selectedPixels(w * h, false) {}

void SelectionLayerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setPen(QPen(m_color));

    for (int y = 0; y < m_h; ++y)
        for (int x = 0; x < m_w; ++x)
            if (m_selectedPixels[y * m_w + x]) painter->drawPoint(x, y);
}

QRectF SelectionLayerItem::boundingRect() const {
    return QRectF{ 0, 0, 1.0 * m_w, 1.0 * m_h };
}

const std::vector<bool> &SelectionLayerItem::selected() const {
    return m_selectedPixels;
}

void SelectionLayerItem::setSize(int w, int h) {
    qDebug() << w << h;
    m_w = w;
    m_h = h;

    // TODO: fix crash during vector deallocation
    m_selectedPixels = std::vector<bool>(w * h, false);
}

void SelectionLayerItem::setSelected(const std::vector<bool> &sel) {
    for (std::size_t i = 0; i < m_selectedPixels.size(); ++i)
        m_selectedPixels[i] = sel[i];
}

void SelectionLayerItem::flipSelected(const std::vector<bool> &difference) {
    qDebug() << difference.size() << m_selectedPixels.size();
    std::transform(difference.cbegin(), difference.cend(), m_selectedPixels.begin(), m_selectedPixels.begin(),
                   std::bit_xor<>());
}
