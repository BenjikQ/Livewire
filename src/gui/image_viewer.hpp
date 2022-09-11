#ifndef IMAGEVIEWER_HPP
#define IMAGEVIEWER_HPP

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class ImageViewer;
}
QT_END_NAMESPACE

class PaintArea;
class PresaveDialog;

class ImageViewer : public QMainWindow {
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer();

private slots:
    void open();
    void save();
    void saveOutlines();
    void loadOutlines();
    void closePath();
    void undo();

private:
    void loadImage(const QString &filePath);

    PaintArea *paintArea;
    PresaveDialog *presaveDialog;
    Ui::ImageViewer *ui;
};

#endif  // IMAGEVIEWER_HPP
