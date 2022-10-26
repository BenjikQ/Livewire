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
        m_oldPathPts = m_seq.nextPath->getPoints();
    }
}

DeleteCommand::~DeleteCommand() {}

void DeleteCommand::undo() {
    adjustSceneIndexing(true);
    for (auto item : ItemPtrList{ m_seq.prevPath, m_seq.thisPoint }) {
        if (item) {
            m_scene->addItem(item);
        }
    }
    if (!m_newPathPts.empty()) {
        m_seq.nextPath->setPoints(m_oldPathPts);
    } else if (m_seq.nextPath) {
        m_seq.nextPath->show();
    }
    ++m_numberOfPoints;
    m_scene->update();
}

void DeleteCommand::redo() {
    for (auto item : ItemPtrList{ m_seq.prevPath, m_seq.thisPoint }) {
        if (item) {
            m_scene->removeItem(item);
        }
    }
    if (!m_newPathPts.empty()) {
        m_seq.nextPath->setPoints(m_newPathPts);
    } else if (m_seq.nextPath) {
        m_seq.nextPath->hide();
    }
    --m_numberOfPoints;
    adjustSceneIndexing();
    m_scene->update();
}

void DeleteCommand::adjustSceneIndexing(bool reverse) {
    const int delIndex = m_seq.thisPoint->number;
    const int diff = reverse ? 1 : -1;

    qDebug() << "DEL INDEX" << delIndex;
    qDebug() << "POINT COUNT" << m_numberOfPoints;

    for (auto *item : m_scene->items()) {
        if (PointItem *foundItem = qgraphicsitem_cast<PointItem *>(item)) {
            if ((foundItem->number > delIndex) || (reverse && (foundItem->number == delIndex))) {
                qDebug() << "POINT" << foundItem->number << "->" << foundItem->number + diff;
                foundItem->number += diff;
            }
        } else if (PathItem *foundItem = qgraphicsitem_cast<PathItem *>(item)) {
            if (foundItem->number != PathItem::ACTIVE_PATH_ID &&
                ((delIndex == 0 && m_seq.prevPoint == nullptr) || (foundItem->number > delIndex) ||
                 (reverse && (foundItem->number == delIndex)))) {
                const int oldNumber = foundItem->number;
                foundItem->number += diff;

                // special case for deleting PointItem 0 with closed path
                if (!reverse && foundItem->number == 0 && m_seq.prevPoint != nullptr)
                    foundItem->number = m_numberOfPoints;
                else if (reverse && (foundItem->number == m_numberOfPoints + 1) && m_seq.prevPoint != nullptr)
                    foundItem->number = 1;

                qDebug() << "PATH" << oldNumber << "->" << foundItem->number;
            }
        }
    }
}
