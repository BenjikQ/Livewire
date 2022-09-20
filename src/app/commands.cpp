#include "commands.hpp"

#include <QGraphicsScene>

#include "painter_options.hpp"
#include "path_item.hpp"
#include "point_item.hpp"

AddCommand::AddCommand(const QList<QPoint> &points, const PainterOptions &options, int &numberOfPoints,
                       QGraphicsScene *scene, QUndoCommand *parent) :
    QUndoCommand{ parent },
    m_scene{ scene },
    m_path{ points.empty() ? nullptr : new PathItem(options, points) },
    m_item{ new PointItem(options) },
    m_numberOfPoints{ numberOfPoints } {}

AddCommand::~AddCommand() {
    if (!m_item->scene()) {
        delete m_item;
    }
    if (m_path && !m_path->scene()) {
        delete m_path;
    }
}

void AddCommand::undo() {
    m_scene->removeItem(m_item);
    if (m_path) {
        m_scene->removeItem(m_path);
    }
    m_scene->update();
    --m_numberOfPoints;
}

void AddCommand::redo() {
    m_scene->addItem(m_item);
    if (m_path) {
        m_scene->addItem(m_path);
    }
    m_scene->update();
    ++m_numberOfPoints;
}
