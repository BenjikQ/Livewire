#include <QApplication>

#include "config.hpp"
#include "main_window.hpp"

static const QString PROJECT{ QString::fromStdString(std::string(PROJECT_NAME)) };
static const QString VERSION{ QString::fromStdString(std::string(PROJECT_VERSION)) };

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setStyle("fusion");  // Due to not displaying underline in menu bar

    QCoreApplication::setApplicationName(PROJECT);
    QCoreApplication::setApplicationVersion(VERSION);

    MainWindow window;
    window.show();

    return QApplication::exec();
}
