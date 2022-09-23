#include "main_window.hpp"

#include <algorithm>

#include <QColorDialog>
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
    auto sceneMousePosition = [event]() {
        const auto *mouseMoveEvent = static_cast<const QGraphicsSceneMouseEvent *>(event);
        const double x{ mouseMoveEvent->scenePos().x() };
        const double y{ mouseMoveEvent->scenePos().y() };
        return QPoint{ static_cast<int>(x), static_cast<int>(y) };
    };
    if (watched == m_scene && event->type() == QEvent::GraphicsSceneMouseMove) {
        m_mouseLastScenePosition = sceneMousePosition();
        updateLabel(m_mouseLastScenePosition);

        if (m_drawing && m_numberOfPoints > 0) {
            drawPath(m_mouseLastScenePosition);
        }
    } else if (m_drawing && watched == m_scene && event->type() == QEvent::GraphicsSceneMousePress &&
               static_cast<const QGraphicsSceneMouseEvent *>(event)->button() == Qt::LeftButton) {
        clickPoint(sceneMousePosition());
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
    if (m_numberOfPoints == 0) {
        m_scene->removeItem(m_path);
        // Not sure if it should be first deleted
        // but then the program crashes
        m_path = nullptr;
    }
    drawPath(m_mouseLastScenePosition);
}

[[maybe_unused]] void MainWindow::redo() {
    m_undoStack->redo();
    drawPath(m_mouseLastScenePosition);
}

void MainWindow::showPointColorDialog() {
    m_painterOptions.pointColor = QColorDialog::getColor();
}

void MainWindow::showPathColorDialog() {
    m_painterOptions.pathColor = QColorDialog::getColor();
}

void MainWindow::setupUi() {
    m_ui->setupUi(this);
    new QShortcut(QKeySequence::Close, this, SLOT(close()));
    new QShortcut(Qt::Key_Return, this, SLOT(closePath()));
    setWindowIcon(QIcon{ ":/icons/data/icons/app.png" });
    setWindowTitle(QCoreApplication::applicationName());
}

void MainWindow::setupSceneText() {
    m_image = {};
    m_numberOfPoints = 0;
    if (m_path && m_path->scene()) {
        m_scene->removeItem(m_path);
        m_path = nullptr;
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
        m_path = nullptr;
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
    const QPoint sceneOffset{ size().width() / 2 - 88, size().height() / 2 - 66 };
    const QPointF mousePosition = m_image.isNull() ? position + sceneOffset : position;
    const QString coordinates = QString::number(mousePosition.x()) + ", " + QString::number(mousePosition.y()) + " pix";
    m_mouseCoordinatesLabel->setText(coordinates);
}

void MainWindow::clickPoint(const QPoint &position, bool final) {
    if (!pointInImage(position)) return;

    m_painterOptions.position = position;
    qDebug() << position.x() << position.y();
    QUndoCommand *addPointCommand = new AddCommand(m_path ? m_path->getPoints() : QList<QPoint>{}, m_painterOptions,
                                                   m_numberOfPoints, m_scene, final ? &m_drawing : nullptr);
    m_undoStack->push(addPointCommand);
}

void MainWindow::drawPath(const QPoint &position) {
    QPointF start{};
    // Get the last clicked point in the scene
    for (const auto *item : m_scene->items()) {
        if (qgraphicsitem_cast<const QGraphicsEllipseItem *>(item)) {
            start = item->pos();
            break;
        }
    }

    if (start.isNull()) return;

    const Point source{ static_cast<int>(start.x()), static_cast<int>(start.y()) };
    const Point destination{ static_cast<int>(position.x()), static_cast<int>(position.y()) };

    if (!pointInImage(source) || !pointInImage(destination)) return;

    const auto path{ m_graph->shortestPath(source, destination) };
    QList<QPoint> points;
    points.reserve(path.size());
    for (auto point : path) {
        points.emplaceBack(point.x, point.y);
    }

    if (!m_path) {
        m_path = new PathItem(m_painterOptions, points);
        m_scene->addItem(m_path);
    } else {
        m_path->setOptions(m_painterOptions);
        m_path->setPoints(points);
        if (!m_path->scene()) {
            m_scene->addItem(m_path);
        }
    }

    m_scene->update();
}

void MainWindow::closePath() {
    if (!m_drawing || m_numberOfPoints <= 1) return;

    QPoint firstPoint;
    const auto items = m_scene->items();

    for (int i = items.size() - 1; i >= 0; --i) {
        if (qgraphicsitem_cast<const QGraphicsEllipseItem *>(items[i])) {
            firstPoint = items[i]->pos().toPoint();
            break;
        }
    }

    drawPath(firstPoint);
    clickPoint(firstPoint, true);
}

bool MainWindow::pointInImage(Point point) const {
    return point.x >= 0 && point.y >= 0 && point.x < m_image.width() && point.y < m_image.height();
}

bool MainWindow::pointInImage(QPoint point) const {
    return point.x() >= 0 && point.y() >= 0 && point.x() < m_image.width() && point.y() < m_image.height();
}
