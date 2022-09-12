#include "commands.hpp"

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QPen>
#include <QRectF>
#include <QSizeF>

AddCommand::AddCommand(const QPointF &position, int &numberOfPoints, QGraphicsScene *scene, QUndoCommand *parent) :
    QUndoCommand{ parent },
    m_numberOfPoints{ numberOfPoints },
    m_scene{ scene },
    topLeft{ position - QPointF{ m_pointRadius / 2, m_pointRadius / 2 } } {
//    const QSizeF size{ m_pointRadius, m_pointRadius };

//    const QRectF rect{ { 0, 0 }, size };  // First argument needs to be null point

//    const QBrush inner{ m_pointInnerColor };
//    const QBrush outer{ m_pointOuterColor };
//    const QPen pen{ outer, m_pointWidth };

    //    m_item = m_scene->addEllipse(rect, pen, inner);
    m_item = new QGraphicsEllipseItem();
}

AddCommand::~AddCommand() {
    if (!m_item->scene()) {
        delete m_item;
    }
}

void AddCommand::undo() {
    m_scene->removeItem(m_item);
    m_scene->update();
    --m_numberOfPoints;
}

void AddCommand::redo() {
    const QBrush inner{ m_pointInnerColor };
    const QBrush outer{ m_pointOuterColor };
    const QPen pen{ outer, m_pointWidth };
    const QSizeF size{ m_pointRadius, m_pointRadius };
    const QRectF rect{ { 0, 0 }, size };  // First argument needs to be null point
    m_item->setPen(pen);
    m_item->setBrush(inner);
    m_item->setPos(topLeft);
    m_item->setRect(rect);
    m_scene->addItem(m_item);
    m_scene->update();
    ++m_numberOfPoints;
}
