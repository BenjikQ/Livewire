#ifndef LIVEWIRE_COMMANDS_HPP
#define LIVEWIRE_COMMANDS_HPP

#include <QColor>
#include <QPointF>
#include <QUndoCommand>

QT_BEGIN_NAMESPACE
class QGraphicsEllipseItem;
class QGraphicsScene;
class QUndoCommand;
QT_END_NAMESPACE

class AddCommand : public QUndoCommand {
public:
    AddCommand(const QPointF &position, int &numberOfPoints, QGraphicsScene *scene, QUndoCommand *parent = nullptr);
    ~AddCommand() override;

    void undo() override;
    void redo() override;

private:
    QGraphicsScene *m_scene;
    QGraphicsEllipseItem *m_item;
    int& m_numberOfPoints;

    float m_pointRadius = 8;
    float m_pointWidth = 1;
    QPointF topLeft{};
    QColor m_pointInnerColor = QColorConstants::Red;
    QColor m_pointOuterColor = QColorConstants::Black;
};

#endif  // LIVEWIRE_COMMANDS_HPP
