#include <QApplication>

#include "imageviewer.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ImageViewer imageViewer;
    imageViewer.show();
    return a.exec();
}
