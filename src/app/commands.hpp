#ifndef LIVEWIRE_COMMANDS_HPP
#define LIVEWIRE_COMMANDS_HPP

#include <QColor>
#include <QPointF>
#include <QUndoCommand>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QUndoCommand;
QT_END_NAMESPACE

class PointItem;
struct PainterOptions;

class AddCommand : public QUndoCommand {
public:
    AddCommand(const PainterOptions &options, int &numberOfPoint, QGraphicsScene *scene,
               QUndoCommand *parent = nullptr);
    ~AddCommand() override;

    void undo() override;
    void redo() override;

private:
    QGraphicsScene *m_scene;
    PointItem *m_item;
    int &m_numberOfPoints;
};

#endif  // LIVEWIRE_COMMANDS_HPP
