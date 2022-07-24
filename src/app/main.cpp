#include <QApplication>

#include "image_viewer.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ImageViewer imageViewer;
    imageViewer.show();
    return a.exec();
}
