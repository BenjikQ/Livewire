#include "commands.hpp"

#include <QGraphicsScene>

#include "painter_options.hpp"
#include "point_item.hpp"

AddCommand::AddCommand(const PainterOptions &options, int &numberOfPoints, QGraphicsScene *scene,
                       QUndoCommand *parent) :
    QUndoCommand{ parent },
    m_scene{ scene },
    m_item{ new PointItem(options) },
    m_numberOfPoints{ numberOfPoints } {}

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
    m_scene->addItem(m_item);
    m_scene->update();
    ++m_numberOfPoints;
}
