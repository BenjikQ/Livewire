#include "main_window.hpp"

#include <iostream>

#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QIcon>
#include <QImageReader>
#include <QImageWriter>
#include <QKeySequence>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPointF>
#include <QShortcut>
#include <QStandardPaths>
#include <QString>
#include <QStyle>
#include <QUndoStack>

#include "commands.hpp"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{ parent },
    m_ui{ new Ui::MainWindow },
    m_scene{ new QGraphicsScene{ this } },
    m_undoStack{ new QUndoStack{ this } } {
    setupSceneAndView();
    setupIcons();
    setupStatusBar();
}

MainWindow::~MainWindow() {
    if (m_line && !m_line->scene()) {
        delete m_line;
    }
    delete m_undoStack;
    delete m_scene;
    delete m_ui;
}

// Capture mouse move over scene
// in order to update label with a mouse position
bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_scene && event->type() == QEvent::GraphicsSceneMouseMove) {
        const auto *mouseMoveEvent = static_cast<const QGraphicsSceneMouseEvent *>(event);
        updateLabel(mouseMoveEvent->scenePos());
        if (m_drawing) {
            if (m_numberOfPoints > 0) {  // When point was clicked we can now draw a line
                drawEdge(mouseMoveEvent->scenePos());
            } else if (m_line && m_line->scene()) {  // When there are fewer points we might need to remove the line
                m_scene->removeItem(m_line);
            }
        }
    } else if (m_drawing && watched == m_scene && event->type() == QEvent::GraphicsSceneMousePress) {
        const auto *mousePressEvent = static_cast<const QGraphicsSceneMouseEvent *>(event);
        clickPoint(mousePressEvent->scenePos());
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

    QMessageBox confirmExit{ QMessageBox::Question, "Confirm Exit", "Are you sure you want to exit?",
                             QMessageBox::Yes | QMessageBox::No, this };

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
        m_scene->clear();
        m_scene->addPixmap(QPixmap::fromImage(m_image));
        m_scene->setSceneRect(QRectF(0, 0, m_image.width(), m_image.height()));
        m_drawing = true;
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

    if (m_line && m_line->scene()) {
        m_scene->removeItem(m_line);
    }

    m_undoStack->clear();

    m_scene->clear();
    m_scene->setSceneRect(m_startSceneRect);
    const QString openShortcut{ m_ui->actionOpen->shortcut().toString() };
    m_scene->addText("Press " + openShortcut + " to open a file...")->setDefaultTextColor(Qt::white);
    m_drawing = false;
    m_numberOfPoints = 0;

    m_ui->actionSaveAs->setEnabled(false);
    m_ui->actionCloseFile->setEnabled(false);

    m_ui->view->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_ui->view->show();
}

// We need to remove drawn line when there are less than two points
// We do it when total number of items in scene is equal three,
// because then there are only an image, currently drawn line one point
[[maybe_unused]] void MainWindow::undo() {
    m_undoStack->undo();
}

[[maybe_unused]] void MainWindow::redo() {
    m_undoStack->redo();
}

void MainWindow::setupSceneAndView() {
    m_ui->setupUi(this);

    new QShortcut(QKeySequence::Close, this, SLOT(close()));

    const QString openShortcut{ m_ui->actionOpen->shortcut().toString() };
    m_scene->addText("Press " + openShortcut + " to open a file...")->setDefaultTextColor(Qt::white);
    m_scene->installEventFilter(this);

    m_startSceneRect = m_scene->sceneRect();

    m_ui->view->setScene(m_scene);
    m_ui->view->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_ui->view->show();
}

void MainWindow::setupIcons() {
    QIcon icon{ ":/icons/data/icons/new.png" };  // Should be set in designer
    m_ui->actionOpen->setIcon(icon);

    icon = QIcon{ ":/icons/data/icons/save.png" };
    m_ui->actionSaveAs->setIcon(icon);

    icon = QIcon{ ":/icons/data/icons/undo.png" };
    m_ui->actionUndo->setIcon(icon);

    icon = QIcon{ ":/icons/data/icons/redo.png" };
    m_ui->actionRedo->setIcon(icon);

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

void MainWindow::updateLabel(const QPointF &position) {
    // if image was not read we need to calculate position in different way
    // because the (0, 0) is now next to the center of the scene
    // thus displaying negative values in left-top part when moving cursor
    // it was tested empirically
    // should have been calculated based on scene intro text
    const QPoint sceneOffset{ size().width() / 2 - 191, size().height() / 2 - 48 };
    const QPointF mousePosition = m_image.isNull() ? position + sceneOffset : position;
    const QString coordinates = QString::number(mousePosition.x()) + ", " + QString::number(mousePosition.y()) + " pix";
    m_mouseCoordinatesLabel->setText(coordinates);
}

void MainWindow::clickPoint(const QPointF &position) {
    QUndoCommand *addPointCommand = new AddCommand(position, m_numberOfPoints, m_scene);
    m_undoStack->push(addPointCommand);
}

void MainWindow::drawEdge(const QPointF &position) {
    constexpr float pointRadius = 8;  // TODO: Set drawing parameter only in one place
    // For some reason when there is only one point on the scene it returns position of image
    // In other cases it returns position of last point
    const QPointF pointPosition{ m_numberOfPoints == 1 ? m_scene->items()[1]->pos() : m_scene->items().first()->pos() };
    const QPointF begin{ pointPosition + QPointF{ pointRadius / 2, pointRadius / 2 } };
    const QLineF line{ begin, position };
    const QBrush brush{ QColorConstants::Red };
    const QPen pen{ brush, 4 };
    if (!m_line) {
        m_line = m_scene->addLine(line, pen);
    } else {
        if (!m_line->scene()) {
            m_scene->addItem(m_line);
        }
        m_line->setLine(line);
    }
}
