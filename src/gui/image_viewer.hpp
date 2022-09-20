#ifndef IMAGEVIEWER_HPP
#define IMAGEVIEWER_HPP

#include <QMainWindow>

#include "typedefs.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class ImageViewer;
}
QT_END_NAMESPACE

class PaintArea;
class PresaveDialog;
class ComparisonDisplay;

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
    void loadOutlinesWithColors();
    void closePath();
    void undo();
    void compareJaccard();

    void zoomIn();
    void zoomOut();
    void setDrawingMode();
    void setEditingMode();

private:
    void loadImage(const QString &filePath);

    PaintArea *paintArea;
    PresaveDialog *presaveDialog;
    ComparisonDisplay *compDisplay;
    Ui::ImageViewer *ui;
};

#endif  // IMAGEVIEWER_HPP
