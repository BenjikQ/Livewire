#include "main_window.hpp"

#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QList>
#include <QStandardPaths>
#include <QString>

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{ parent },
    m_ui{ new Ui::MainWindow } {
    m_ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete m_ui;
}

void MainWindow::open() {
    static const QString caption = tr("Open Image");
    static const QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory = homePath.first().split(QDir::separator()).last();
    static const QString filter = tr("Image Files (*.png *.jpg *.bmp)");

    const QString filePath = QFileDialog::getOpenFileName(this, caption, homeDirectory, filter);
    if (!filePath.isEmpty()) {
        QImageReader reader{ filePath };
        m_image = reader.read();
        setMouseTracking(true);
        m_ui->imageLabel->setPixmap(QPixmap::fromImage(m_image));
        m_ui->imageLabel->adjustSize();
    }
}
