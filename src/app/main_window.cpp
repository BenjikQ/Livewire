#include "main_window.hpp"

#include <QDir>
#include <QFileDialog>
#include <QList>
#include <QStandardPaths>
#include <QString>

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_paintArea(new PaintArea(this)) {
    m_ui->setupUi(this);
    m_ui->scrollArea->setVisible(false);
    m_ui->scrollArea->setWidget(m_paintArea);
}

MainWindow::~MainWindow() {
    delete m_paintArea;
    delete m_ui;
}

void MainWindow::open() {
    static const QString caption = tr("Open Image");
    static const QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory = homePath.first().split(QDir::separator()).last();
    static const QString filter = tr("Image Files (*.png *.jpg *.bmp)");

    const QString filePath = QFileDialog::getOpenFileName(this, caption, homeDirectory, filter);
    if (!filePath.isEmpty()) {
        m_paintArea->open(filePath);
        m_ui->scrollArea->setVisible(true);
    }
}
