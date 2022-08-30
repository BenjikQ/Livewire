#ifndef IMAGEVIEWER_HPP
#define IMAGEVIEWER_HPP

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class ImageViewer;
}
QT_END_NAMESPACE

class PaintArea;

class ImageViewer : public QMainWindow {
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer();

private slots:
    void open();
    void save();
    void closePath();

private:
    void loadImage(const QString &filePath);

    PaintArea *paintArea;
    Ui::ImageViewer *ui;
};

#endif  // IMAGEVIEWER_HPP
