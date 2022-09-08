#include <string>

#include <QApplication>

#include "config.hpp"
#include "main_window.hpp"

static const QString PROJECT = QString::fromStdString(std::string(PROJECT_NAME));
static const QString VERSION = QString::fromStdString(std::string(PROJECT_VERSION));

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(PROJECT);
    QCoreApplication::setApplicationVersion(VERSION);

    MainWindow window;
    window.setWindowTitle(PROJECT);
    window.show();

    return app.exec();
}
