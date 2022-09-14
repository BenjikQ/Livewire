#include "image_label.hpp"

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent) {}

void ImageLabel::resizeEvent(QResizeEvent *pQEvent) {
    QLabel::resizeEvent(pQEvent);
    setPixmap(_qPixmap, pQEvent->size());
}

void ImageLabel::setPixmap(const QPixmap &qPixmap, const QSize &size) {
    _qPixmap = qPixmap;
    _qPixmapScaled = _qPixmap.scaled(size, Qt::KeepAspectRatio);
    QLabel::setPixmap(_qPixmapScaled);
}
