#ifndef PAINTAREA_H
#define PAINTAREA_H

#include <QImage>
#include <QString>
#include <QWidget>

class PaintArea : public QWidget {
    Q_OBJECT

public:
    explicit PaintArea(QWidget *parent = nullptr);

    void open(const QString &filePath);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage m_image;
};

#endif  // PAINTAREA_H
