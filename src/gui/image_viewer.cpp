#include "image_viewer.hpp"

#include "./ui_image_viewer.h"

ImageViewer::ImageViewer(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::ImageViewer) {
    ui->setupUi(this);
}

ImageViewer::~ImageViewer() { delete ui; }
