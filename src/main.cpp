#include "imageviewer.h"
#include "lw_utils.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    return debugMain();

    QApplication a(argc, argv);
    ImageViewer imageViewer;
    imageViewer.show();
    return a.exec();
}
