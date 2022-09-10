#include "diagram_scene.hpp"

#include <QBrush>
#include <QCursor>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QLineF>
#include <QPen>
#include <QPointF>
#include <QRectF>

DiagramScene::DiagramScene(QObject *parent) :
    QGraphicsScene{ parent } {}

void DiagramScene::reset() {
    clear();
    m_lastPoint = nullptr;
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (m_drawing) {
        if (mouseEvent->button() == Qt::LeftButton) {
            if (m_lastPoint) {  // There must be at least one point in order to draw an edge
                addEdge(mouseEvent->scenePos());
            }
            addPoint(mouseEvent->scenePos());
        }
    }
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (m_drawing && m_lastPoint) {
    }
}

void DiagramScene::addPoint(const QPointF &position) {
    const QPointF topLeft{ position - QPointF{ m_pointRadius / 2, m_pointRadius / 2 } };
    const QSizeF size{ m_pointRadius, m_pointRadius };
    const QRectF rect{ { 0, 0 }, size };  // First argument needs to be null point

    const QBrush inner{ m_pointInnerColor };
    const QBrush outer{ m_pointOuterColor };
    const QPen pen{ outer, m_pointWidth };

    m_lastPoint = addEllipse(rect, pen, inner);
    m_lastPoint->setPos(topLeft);  // then we manually set a desired location
}

void DiagramScene::addEdge(const QPointF &position) {
    const QPointF begin{ m_lastPoint->scenePos() + QPointF{ m_pointRadius / 2, m_pointRadius / 2 } };
    const QLineF line{ begin, position };

    const QBrush brush{ m_lineColor };
    const QPen pen{ brush, m_lineWidth };

    addLine(line, pen);
}
