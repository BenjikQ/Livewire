#ifndef LIVEWIRE_COMMANDS_HPP
#define LIVEWIRE_COMMANDS_HPP

#include <unordered_set>
#include <vector>

#include <QColor>
#include <QPointF>
#include <QUndoCommand>

#include "path_sequence.hpp"
#include "selection_layer_item.hpp"

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QUndoCommand;
QT_END_NAMESPACE

struct PainterOptions;

class AddCommand : public QUndoCommand {
public:
    AddCommand(const QList<QPoint> &points, const PainterOptions &options, int &numberOfPoint, QGraphicsScene *scene,
               bool *drawing, QUndoCommand *parent = nullptr);
    ~AddCommand() override;

    void undo() override;
    void redo() override;

private:
    QGraphicsScene *m_scene;
    PathItem *m_path;
    PointItem *m_item;
    int &m_numberOfPoints;
    bool *m_drawing;
};

class DeleteCommand : public QUndoCommand {
    using ItemPtrList = std::initializer_list<QGraphicsItem *>;

public:
    DeleteCommand(const PathSequence &seq, const QList<QPoint> &points, const PainterOptions &options,
                  int &numberOfPoints, QGraphicsScene *scene, QUndoCommand *parent = nullptr);
    ~DeleteCommand() override;

    void undo() override;
    void redo() override;

private:
    QList<QPoint> m_newPathPts{}, m_oldPathPts{};
    PathSequence m_seq;
    QGraphicsScene *m_scene;
    int &m_numberOfPoints;

    void adjustSceneIndexing(bool reverse = false);
};

class RegionSelectCommand : public QUndoCommand {
public:
    RegionSelectCommand(QPoint origin, const std::unordered_set<QPoint> &outline, QGraphicsScene *scene,
                        SelectionLayerItem *selItem, QUndoCommand *parent = nullptr);
    ~RegionSelectCommand() override;

    void undo() override;
    void redo() override;

private:
    std::vector<bool> m_stateChange;
    QGraphicsScene *m_scene;
    SelectionLayerItem *m_selection;
};

#endif  // LIVEWIRE_COMMANDS_HPP
