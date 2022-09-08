#ifndef DIAGRAMSCENE_HPP
#define DIAGRAMSCENE_HPP

#include <QGraphicsScene>
#include <QColor>

class DiagramScene : public QGraphicsScene {
public:
    DiagramScene(QObject *parent = nullptr);

    void enableDrawing(bool drawing) { m_drawing = drawing; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private:
    void drawPoint(int x, int y);

private:
    bool m_drawing = false;

    float m_pointRadius = 8;
    QColor m_pointColor = QColorConstants::Red;
};

#endif  // DIAGRAMSCENE_HPP
