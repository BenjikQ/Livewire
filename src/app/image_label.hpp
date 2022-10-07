#ifndef IMAGELABEL_HPP
#define IMAGELABEL_HPP

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

// https://stackoverflow.com/a/42852010

class ImageLabel : public QLabel {
    Q_OBJECT
public:
    explicit ImageLabel(QWidget *parent = nullptr);
    void setPixmap(const QPixmap &qPixmap) { setPixmap(qPixmap, size()); }

protected:
    virtual void resizeEvent(QResizeEvent *pQEvent);

private:
    QPixmap _qPixmap, _qPixmapScaled;

    void setPixmap(const QPixmap &qPixmap, const QSize &size);
};


#endif  // IMAGELABEL_HPP
