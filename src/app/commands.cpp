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
    m_path{ points.empty() ? nullptr : new PathItem(numberOfPoints, options, points) },
    m_item{ new PointItem(numberOfPoints, options) },
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
    if (m_path) {
        m_scene->removeItem(m_path);
    }
    if (m_drawing != nullptr) {
        *m_drawing = true;
    } else {
        m_scene->removeItem(m_item);
        --m_numberOfPoints;
    }
    m_scene->update();
}

void AddCommand::redo() {
    if (m_path) {
        m_scene->addItem(m_path);
    }
    if (m_drawing != nullptr) {
        *m_drawing = false;
    } else {
        m_scene->addItem(m_item);
        ++m_numberOfPoints;
    }
    m_scene->update();
}

RegionSelectCommand::RegionSelectCommand(QPoint origin, const std::unordered_set<QPoint> &outline,
                                         QGraphicsScene *scene, SelectionLayerItem *selItem, QUndoCommand *parent) :
    QUndoCommand{ parent },
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

DeleteCommand::DeleteCommand(const PathSequence &seq, const QList<QPoint> &points, const PainterOptions &options,
                             int &numberOfPoints, QGraphicsScene *scene, QUndoCommand *parent) :
    QUndoCommand{ parent },
    m_seq{ seq },
    m_numberOfPoints{ numberOfPoints },
    m_scene{ scene } {
    if (!points.empty()) {
        m_newPathPts = points;
        m_oldPathPts = m_seq.prevPath->getPoints();
    }
}

DeleteCommand::~DeleteCommand() {}

void DeleteCommand::undo() {
    for (auto item : ItemPtrList{ m_seq.nextPath, m_seq.thisPoint }) {
        if (item) {
            m_scene->addItem(item);
        }
    }
    if (!m_newPathPts.empty()) {
        m_seq.prevPath->setPoints(m_oldPathPts);
    }
    ++m_numberOfPoints;
    m_scene->update();
}

void DeleteCommand::redo() {
    for (auto item : ItemPtrList{ m_seq.nextPath, m_seq.thisPoint }) {
        if (item) {
            m_scene->removeItem(item);
        }
    }
    if (!m_newPathPts.empty()) {
        m_seq.prevPath->setPoints(m_newPathPts);
    }
    --m_numberOfPoints;
    m_scene->update();
}
