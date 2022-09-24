#include "commands.hpp"

#include <functional>

#include <QGraphicsScene>

#include "imgprc_helpers.hpp"
#include "painter_options.hpp"
#include "path_item.hpp"
#include "point_item.hpp"

AddCommand::AddCommand(const QList<QPoint> &points, const PainterOptions &options, int &numberOfPoints,
                       QGraphicsScene *scene, bool *drawing, QUndoCommand *parent) :
    QUndoCommand{ parent },
    m_scene{ scene },
    m_path{ points.empty() ? nullptr : new PathItem(options, points) },
    m_item{ new PointItem(options) },
    m_numberOfPoints{ numberOfPoints },
    m_drawing{ drawing } {}

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
    if (m_drawing != nullptr) {
        *m_drawing = true;
    }
    m_scene->update();
    --m_numberOfPoints;
}

void AddCommand::redo() {
    m_scene->addItem(m_item);
    if (m_path) {
        m_scene->addItem(m_path);
    }
    if (m_drawing != nullptr) {
        *m_drawing = false;
    }
    m_scene->update();
    ++m_numberOfPoints;
}

RegionSelectCommand::RegionSelectCommand(QPoint origin, const std::unordered_set<QPoint> &outline,
                                         QGraphicsScene *scene, SelectionLayerItem *selItem, QUndoCommand *parent) :
    m_selection{ selItem },
    m_scene{ scene } {
    m_stateChange = floodFill(selItem->selected(), outline, origin, selItem->w(), selItem->h());
}

RegionSelectCommand::~RegionSelectCommand() {}

void RegionSelectCommand::undo() {
    redo();
}

void RegionSelectCommand::redo() {
    m_selection->flipSelected(m_stateChange);
    m_scene->update();
}
