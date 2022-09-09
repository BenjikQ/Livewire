#include "main_window.hpp"

#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QIcon>
#include <QImageReader>
#include <QLabel>
#include <QList>
#include <QMouseEvent>
#include <QStandardPaths>
#include <QString>
#include <QStyle>

#include "diagram_scene.hpp"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{ parent },
    m_ui{ new Ui::MainWindow },
    m_scene{ new DiagramScene(this) } {
    m_ui->setupUi(this);
    m_scene->installEventFilter(this);
    m_ui->view->setScene(m_scene);
    setupStatusBar();
}

MainWindow::~MainWindow() {
    delete m_ui;
}

// Capture mouse move over scene
// in order to update label with a mouse position
bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_scene && event->type() == QEvent::GraphicsSceneMouseMove) {
        const QGraphicsSceneMouseEvent *mouseMoveEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        const QPointF mousePosition = mouseMoveEvent->scenePos();
        const QString coordinates =
            QString::number(mousePosition.x()) + ", " + QString::number(mousePosition.y()) + " pix";
        m_mouseCoordinatesLabel->setText(coordinates);
    }
    return false;
}

// Clearing label when mouse left scene
// Remember to set mouseTracking in all sub widgets
// in ui designer or by calling setMouseTracking(true)
void MainWindow::mouseMoveEvent(QMouseEvent *mouseEvent) {
    m_mouseCoordinatesLabel->clear();
}

void MainWindow::open() {
    static const QString caption = tr("Open Image");
    static const QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory = homePath.first().split(QDir::separator()).last();
    static const QString filter = tr("Image Files (*.png *.jpg *.bmp)");

    const QString filePath = QFileDialog::getOpenFileName(this, caption, homeDirectory, filter);
    if (!filePath.isEmpty()) {
        QFileInfo info{ filePath };
        setWindowTitle(QCoreApplication::applicationName() + " - " + info.fileName());

        QImageReader reader{ filePath };
        m_image = reader.read();

        m_scene->reset();
        m_scene->addPixmap(QPixmap::fromImage(m_image));
        m_scene->setSceneRect(QRectF(0, 0, m_image.width(), m_image.height()));
        m_scene->enableDrawing(true);
    }
}

void MainWindow::setupStatusBar() {
    m_mouseCoordinatesIcon = new QLabel(this);
    m_mouseCoordinatesIcon->setStyleSheet("margin-bottom: 10px; margin-left: 10px");
    QIcon icon{ ":/icons/data/images/mouse-coordinates.png" };
    QPixmap pixmap = icon.pixmap(QSize{ 16, 16 });
    m_mouseCoordinatesIcon->setPixmap(pixmap);

    m_mouseCoordinatesLabel = new QLabel(this);
    m_mouseCoordinatesLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_mouseCoordinatesLabel->setStyleSheet("margin-bottom: 10px");

    statusBar()->addWidget(m_mouseCoordinatesIcon);
    statusBar()->addWidget(m_mouseCoordinatesLabel);
}
