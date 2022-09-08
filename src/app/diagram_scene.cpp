#include "diagram_scene.hpp"

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

DiagramScene::DiagramScene(QObject *parent) :
    QGraphicsScene{ parent } {}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (m_drawing) {
        if (mouseEvent->button() == Qt::LeftButton) {
            drawPoint(mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
        }
    }
}

void DiagramScene::drawPoint(int x, int y) {
    const QPointF topLeft{ x - m_pointRadius / 2, y - m_pointRadius / 2 };
    const QSizeF size{ m_pointRadius, m_pointRadius };
    QGraphicsEllipseItem *item = new QGraphicsEllipseItem(QRectF{ topLeft, size });
    item->setBrush(m_pointColor);
    addItem(item);
}
