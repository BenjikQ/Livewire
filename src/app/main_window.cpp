#include "main_window.hpp"

#include <algorithm>

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
#include <QPoint>
#include <QShortcut>
#include <QStandardPaths>
#include <QString>
#include <QStyle>
#include <QUndoStack>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "commands.hpp"
#include "painter_options.hpp"
#include "ui_main_window.h"

static const QString caption{ "Open Image" };
static const QStringList homePath{ QStandardPaths::standardLocations(QStandardPaths::HomeLocation) };
static const QString homeDirectory{ homePath.first().split(QDir::separator()).last() };
static const QString filter{ "Image Files (*.png *.jpg *.bmp)" };

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{ parent },
    m_ui{ new Ui::MainWindow },
    m_scene{ new QGraphicsScene{ this } },
    m_undoStack{ new QUndoStack{ this } } {
    setupUi();
    setupSceneText();
    setupTextView();
    setupStatusBar();
    setupActions(false);
    m_initialSceneRect = m_scene->sceneRect();
}

MainWindow::~MainWindow() {
    if (m_path && !m_path->scene()) {
        delete m_path;
    }
    delete m_undoStack;
    delete m_scene;
    delete m_ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_scene && event->type() == QEvent::GraphicsSceneMouseMove) {
        const auto *mouseMoveEvent = static_cast<const QGraphicsSceneMouseEvent *>(event);
        const double x{ mouseMoveEvent->scenePos().x() };
        const double y{ mouseMoveEvent->scenePos().y() };
        const QPoint mousePosition{ static_cast<int>(x), static_cast<int>(y) };
        updateLabel(mousePosition);

        if (m_drawing) {
            if (m_numberOfPoints > 0) {
                drawPath(mousePosition);
            } else if (m_path && m_path->scene()) {
                m_scene->removeItem(m_path);
            }
        }
    } else if (m_drawing && watched == m_scene && event->type() == QEvent::GraphicsSceneMousePress) {
        const auto *mousePressEvent = static_cast<const QGraphicsSceneMouseEvent *>(event);
        const double x{ mousePressEvent->scenePos().x() };
        const double y{ mousePressEvent->scenePos().y() };
        const QPoint mousePosition{ static_cast<int>(x), static_cast<int>(y) };
        clickPoint(mousePosition);
    }
    return false;
}

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
    const QString filePath = QFileDialog::getOpenFileName(this, caption, homeDirectory, filter);
    if (!filePath.isEmpty()) {
        QImageReader reader{ filePath };
        m_image = reader.read();
        cv::Mat temp = cv::imread(filePath.toStdString());
        cv::cvtColor(temp, m_imageGray, cv::COLOR_BGR2GRAY);
        m_graph = std::make_unique<DiagonalGraph<CostFunction>>(CostFunction{}, m_imageGray);
        setupSceneImage();
        setupImageView();
        const QFileInfo info{ filePath };
        setWindowTitle(QCoreApplication::applicationName() + " - " + info.fileName());
        setupActions(true);
    }
}

[[maybe_unused]] void MainWindow::saveImageFile() {
    const QString filePath = QFileDialog::getSaveFileName(this, caption, homeDirectory, filter);
    if (!filePath.isEmpty()) {
        QImageWriter writer{ filePath };
        QImage resultImage{ m_image };
        writer.write(resultImage);
    }
}

[[maybe_unused]] void MainWindow::closeImageFile() {
    setupSceneText();
    setupTextView();
    setupActions(false);
    setWindowTitle(QCoreApplication::applicationName());
}

[[maybe_unused]] void MainWindow::undo() {
    m_undoStack->undo();
    m_scene->update();
}

[[maybe_unused]] void MainWindow::redo() {
    m_undoStack->redo();
    m_scene->update();
}

void MainWindow::setupUi() {
    m_ui->setupUi(this);
    new QShortcut(QKeySequence::Close, this, SLOT(close()));
    setWindowIcon(QIcon{ ":/icons/data/icons/app.png" });
    setWindowTitle(QCoreApplication::applicationName());
}

void MainWindow::setupSceneText() {
    m_image = {};
    m_numberOfPoints = 0;
    if (m_path && m_path->scene()) {
        m_scene->removeItem(m_path);
    }
    const QString openShortcut{ m_ui->actionOpen->shortcut().toString() };
    m_undoStack->clear();
    m_scene->clear();
    m_scene->addText("Press " + openShortcut + " to open a file...")->setDefaultTextColor(Qt::white);
    m_scene->installEventFilter(this);
    m_scene->setSceneRect(m_initialSceneRect);
    m_drawing = false;
}

void MainWindow::setupSceneImage() {
    m_numberOfPoints = 0;
    if (m_path && m_path->scene()) {
        m_scene->removeItem(m_path);
    }
    m_undoStack->clear();
    m_scene->clear();
    m_scene->addPixmap(QPixmap::fromImage(m_image));
    m_scene->setSceneRect(QRectF(0, 0, m_image.width(), m_image.height()));
    m_drawing = true;
}

void MainWindow::setupTextView() {
    m_ui->view->setScene(m_scene);
    m_ui->view->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_ui->view->show();
}

void MainWindow::setupImageView() {
    m_ui->view->setScene(m_scene);
    m_ui->view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_ui->view->show();
}

void MainWindow::setupStatusBar() {
    createIconLabel(m_mouseCoordinatesIcon, ":/icons/data/icons/mouse-coordinates.png");
    createTextLabel(m_mouseCoordinatesLabel, "", 80);

    createIconLabel(m_screenSizeIcon, ":/icons/data/icons/screen.png");
    const QString windowSize = QString::number(size().width()) + " × " + QString::number(size().height());
    createTextLabel(m_screenSizeLabel, windowSize);
}

void MainWindow::setupActions(bool enabled) {
    m_ui->actionSaveAs->setEnabled(enabled);
    m_ui->actionCloseFile->setEnabled(enabled);
}

void MainWindow::createTextLabel(QLabel *&textLabel, const QString &text, int width) {
    textLabel = new QLabel(this);
    textLabel->setStyleSheet("margin-bottom: 10px");
    if (width > 0) {
        textLabel->setFixedWidth(width);
    }
    textLabel->setText(text);
    statusBar()->addWidget(textLabel);
}

void MainWindow::createIconLabel(QLabel *&iconLabel, const QString &iconPath) {
    iconLabel = new QLabel(this);
    iconLabel->setStyleSheet("margin-bottom: 10px; margin-left: 10px");
    const QIcon icon{ iconPath };
    const QPixmap pixmap = icon.pixmap(QSize{ 16, 16 });
    iconLabel->setPixmap(pixmap);
    statusBar()->addWidget(iconLabel);
}

void MainWindow::updateLabel(const QPoint &position) {
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

void MainWindow::clickPoint(const QPoint &position) {
    QList<QPoint> points;
    if (m_numberOfPoints > 0) {
        QPointF start{};
        for (const auto *item : m_scene->items()) {
            if (qgraphicsitem_cast<const QGraphicsEllipseItem *>(item) && item->pos() != QPoint{}) {
                start = item->pos();
                break;
            }
        }

        const Point source{ static_cast<int>(start.x()), static_cast<int>(start.y()) };
        const Point destination{ static_cast<int>(position.x()), static_cast<int>(position.y()) };
        const auto path{ m_graph->shortestPath(source, destination) };
        points.reserve(path.size());
        for (auto point : path) {
            points.emplaceBack(point.x, point.y);
        }
    }

    const PainterOptions pointOptions{ position, Qt::red, Qt::black, 1, 4 };
    QUndoCommand *addPointCommand = new AddCommand(points, pointOptions, m_numberOfPoints, m_scene);
    m_undoStack->push(addPointCommand);
}

void MainWindow::drawPath(const QPoint &position) {
    QPointF start{};
    for (const auto *item : m_scene->items()) {
        if (qgraphicsitem_cast<const QGraphicsEllipseItem *>(item) && item->pos() != QPoint{}) {
            start = item->pos();
            break;
        }
    }

    const Point source{ static_cast<int>(start.x()), static_cast<int>(start.y()) };
    const Point destination{ static_cast<int>(position.x()), static_cast<int>(position.y()) };
    const auto path{ m_graph->shortestPath(source, destination) };
    QList<QPoint> points;
    points.reserve(path.size());
    for (auto point : path) {
        points.emplaceBack(point.x, point.y);
    }

    if (!m_path) {
        m_path = new PathItem(points);
        m_scene->addItem(m_path);
    } else {
        if (!m_path->scene()) {
            m_scene->addItem(m_path);
        }
        m_path->setPoints(points);
    }

    update();
}
