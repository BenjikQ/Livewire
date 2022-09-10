#ifndef DIAGRAMSCENE_HPP
#define DIAGRAMSCENE_HPP

#include <QColor>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
class QGraphicsEllipseItem;
class QGraphicsSceneMouseEvent;
class QPointF;
QT_END_NAMESPACE

class DiagramScene : public QGraphicsScene {
public:
    explicit DiagramScene(QObject *parent = nullptr);

    void enableDrawing(bool drawing) { m_drawing = drawing; }
    void reset();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private:
    void addPoint(const QPointF &position);
    void addEdge(const QPointF &position);

private:
    bool m_drawing{ false };

    QGraphicsEllipseItem *m_lastPoint{ nullptr };  // last clicked point

    float m_pointRadius{ 8 };
    float m_pointWidth{ 1 };
    QColor m_pointInnerColor{ QColorConstants::Red };
    QColor m_pointOuterColor{ QColorConstants::Black };

    float m_lineWidth{ 4 };
    QColor m_lineColor{ QColorConstants::Green };
};

#endif  // DIAGRAMSCENE_HPP
