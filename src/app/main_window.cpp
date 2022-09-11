#include "main_window.hpp"

#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QIcon>
#include <QImageReader>
#include <QImageWriter>
#include <QKeySequence>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QShortcut>
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

    new QShortcut(QKeySequence::Close, this, SLOT(close()));

    const QString openShortcut{ m_ui->actionOpen->shortcut().toString() };
    m_scene->addText("Press " + openShortcut + " to open a file...")->setDefaultTextColor(Qt::white);
    m_scene->installEventFilter(this);

    m_startSceneRect = m_scene->sceneRect();

    m_ui->view->setScene(m_scene);
    m_ui->view->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_ui->view->show();

    setupIcons();
    setupStatusBar();
}

MainWindow::~MainWindow() {
    delete m_ui;
}

// Capture mouse move over scene
// in order to update label with a mouse position
bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_scene && event->type() == QEvent::GraphicsSceneMouseMove) {
        const QGraphicsSceneMouseEvent *mouseMoveEvent =
            static_cast<QGraphicsSceneMouseEvent *>(event);  // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

        // if image was not read we need to calculate position in different way
        // because the (0, 0) is now next to the center of the scene
        // thus displaying negative values in left-top part when moving cursor
        // it was tested empirically
        // should have been calculated based on scene intro text
        const QPoint sceneOffset{ size().width() / 2 - 191, size().height() / 2 - 48 };
        const QPointF mousePosition =
            m_image.isNull() ? mouseMoveEvent->scenePos() + sceneOffset : mouseMoveEvent->scenePos();
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

void MainWindow::resizeEvent(QResizeEvent *resizeEvent) {
    QMainWindow::resizeEvent(resizeEvent);
    const QString windowSize = QString::number(size().width()) + " × " + QString::number(size().height());
    m_screenSizeLabel->setText(windowSize);
}

void MainWindow::closeEvent(QCloseEvent *closeEvent) {
    closeEvent->ignore();

    QMessageBox confirmExit{ QMessageBox::Question,
                             "Confirm Exit",
                             "Are you sure you want to exit?",
                             QMessageBox::Yes | QMessageBox::No,
                             this };

    if (confirmExit.exec() == QMessageBox::Yes) {
        closeEvent->accept();
    }
}

[[maybe_unused]] void MainWindow::openImageFile() {
    static const QString caption = tr("Open Image");
    static const QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory = homePath.first().split(QDir::separator()).last();
    static const QString filter = tr("Image Files (*.png *.jpg *.bmp)");

    const QString filePath = QFileDialog::getOpenFileName(this, caption, homeDirectory, filter);
    if (!filePath.isEmpty()) {
        const QFileInfo info{ filePath };
        setWindowTitle(QCoreApplication::applicationName() + " - " + info.fileName());

        QImageReader reader{ filePath };
        m_image = reader.read();

        m_ui->actionSaveAs->setEnabled(true);
        m_ui->actionCloseFile->setEnabled(true);

        m_ui->view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        m_scene->reset();
        m_scene->addPixmap(QPixmap::fromImage(m_image));
        m_scene->setSceneRect(QRectF(0, 0, m_image.width(), m_image.height()));
        m_scene->enableDrawing(true);
    }
}

[[maybe_unused]] void MainWindow::saveImageFile() {
    static const QString caption = tr("Save Image");
    static const QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory = homePath.first().split(QDir::separator()).last();
    static const QString filter = tr("Image Files (*.png *.jpg *.bmp)");

    const QString filePath = QFileDialog::getSaveFileName(this, caption, homeDirectory, filter);
    if (!filePath.isEmpty()) {
        QImageWriter writer{ filePath };
        QImage resultImage{ m_image };
        writer.write(resultImage);
    }
}

[[maybe_unused]] void MainWindow::closeImageFile() {
    m_image = {};

    setWindowTitle(QCoreApplication::applicationName());

    m_scene->reset();
    m_scene->setSceneRect(m_startSceneRect);
    const QString openShortcut{ m_ui->actionOpen->shortcut().toString() };
    m_scene->addText("Press " + openShortcut + " to open a file...")->setDefaultTextColor(Qt::white);
    m_scene->enableDrawing(false);

    m_ui->actionSaveAs->setEnabled(false);
    m_ui->actionCloseFile->setEnabled(false);

    m_ui->view->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_ui->view->show();
}

void MainWindow::setupIcons() {
    QIcon icon{ ":/icons/data/icons/new.png" };  // Should be set in designer
    m_ui->actionOpen->setIcon(icon);

    icon = QIcon{ ":/icons/data/icons/save.png" };
    m_ui->actionSaveAs->setIcon(icon);

    icon = QIcon{ ":/icons/data/icons/app.png" };
    setWindowIcon(icon);
}

void MainWindow::setupStatusBar() {
    setupIconsInStatusBar();
    setupLabelsInStatusBar();

    // Order of adding widgets is important
    // as they appear from left to right in the status bar
    statusBar()->addWidget(m_mouseCoordinatesIcon);
    statusBar()->addWidget(m_mouseCoordinatesLabel);
    statusBar()->addWidget(m_screenSizeIcon);
    statusBar()->addWidget(m_screenSizeLabel);
}

void MainWindow::setupIconsInStatusBar() {
    m_mouseCoordinatesIcon = new QLabel(this);
    m_mouseCoordinatesIcon->setStyleSheet("margin-bottom: 10px; margin-left: 10px");
    QIcon icon{ ":/icons/data/icons/mouse-coordinates.png" };
    QPixmap pixmap = icon.pixmap(QSize{ 16, 16 });
    m_mouseCoordinatesIcon->setPixmap(pixmap);

    m_screenSizeIcon = new QLabel(this);
    m_screenSizeIcon->setStyleSheet("margin-bottom: 10px; margin-left: 10px");
    icon = QIcon{ ":/icons/data/icons/screen.png" };
    pixmap = icon.pixmap(QSize{ 16, 16 });
    m_screenSizeIcon->setPixmap(pixmap);
}

void MainWindow::setupLabelsInStatusBar() {
    m_mouseCoordinatesLabel = new QLabel(this);
    m_mouseCoordinatesLabel->setStyleSheet("margin-bottom: 10px");
    m_mouseCoordinatesLabel->setFixedWidth(80);

    m_screenSizeLabel = new QLabel(this);
    m_screenSizeLabel->setStyleSheet("margin-bottom: 10px");
    const QString windowSize = QString::number(size().width()) + " × " + QString::number(size().height());
    m_screenSizeLabel->setText(windowSize);
}
