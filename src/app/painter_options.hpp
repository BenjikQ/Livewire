#ifndef LIVEWIRE_PAINTER_OPTIONS_HPP
#define LIVEWIRE_PAINTER_OPTIONS_HPP

#include <QColor>
#include <QPointF>

struct PainterOptions {
    QPointF position{};
    QColor innerColor{};
    QColor outerColor{};
    float width{};
    float radius{};
};

#endif  // LIVEWIRE_PAINTER_OPTIONS_HPP
