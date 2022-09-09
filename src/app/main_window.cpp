#include "main_window.hpp"

#include <QDir>
#include <QFileDialog>
#include <QGraphicsView>
#include <QImageReader>
#include <QList>
#include <QStandardPaths>
#include <QString>

#include "diagram_scene.hpp"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{ parent },
    m_ui{ new Ui::MainWindow },
    m_scene{ new DiagramScene(this) } {
    m_ui->setupUi(this);
    m_ui->view->setScene(m_scene);
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

        m_scene->reset();
        m_scene->addPixmap(QPixmap::fromImage(m_image));
        m_scene->setSceneRect(QRectF(0, 0, m_image.width(), m_image.height()));
        m_scene->enableDrawing(true);
    }
}
