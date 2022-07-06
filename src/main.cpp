#include "imageviewer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageViewer imageViewer;
    imageViewer.show();
    return a.exec();
}
