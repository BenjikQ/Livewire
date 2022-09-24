#ifndef LIVEWIRE_COMMANDS_HPP
#define LIVEWIRE_COMMANDS_HPP

#include <unordered_set>
#include <vector>

#include <QColor>
#include <QPointF>
#include <QUndoCommand>

#include "selection_layer_item.hpp"

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QUndoCommand;
QT_END_NAMESPACE

class PathItem;
class PointItem;

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
