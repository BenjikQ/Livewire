#ifndef PAINTAREA_HPP
#define PAINTAREA_HPP

#include <QWidget>

class PaintArea : public QWidget {
    Q_OBJECT
public:
    explicit PaintArea(QWidget *parent = nullptr);

    void open(const QString &filePath);
    void save(const QString &filePath);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawLineTo(const QPoint &endPoint);

    bool drawing = false;
    QImage image;
    QPoint lastPoint;
};

#endif  // PAINTAREA_HPP
