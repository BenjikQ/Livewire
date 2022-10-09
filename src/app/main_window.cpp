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
#include <QGraphicsVideoItem>
#include <QIcon>
#include <QImageReader>
#include <QImageWriter>
#include <QKeySequence>
#include <QLabel>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPoint>
#include <QShortcut>
#include <QStandardPaths>
#include <QString>
#include <QStyle>
#include <QTimeLine>
#include <QUndoStack>
#include <QVideoFrame>
#include <QVideoSink>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "commands.hpp"
#include "imgprc_helpers.hpp"
#include "path_item.hpp"
#include "point_item.hpp"
#include "ui_main_window.h"

static const QString caption{ "Open Image" };
static const QStringList homePath{ QStandardPaths::standardLocations(QStandardPaths::HomeLocation) };
static const QString homeDirectory{ homePath.first().split(QDir::separator()).last() };

static const QList<QString> imageFilters{ "png", "jpg", "bmp" };
static const QList<QString> videoFilters{ "mp4", "mkv", "avi" };
static const QString filter{ "Image Files (*.png *.jpg *.bmp);;Video Files (*.mp4 *.mkv *avi)" };

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{ parent },
    m_ui{ new Ui::MainWindow },
    m_scene{ new QGraphicsScene{ this } },
    m_undoStack{ new QUndoStack{ this } },
    m_saveDialog{ new PresaveDialog{ this } },
    m_compDisplay{ new ComparisonDisplay{ this } } {
    setupUi();
    setupSceneText();
    setupTextView();
    setupStatusBar();
    setupActions(false);
    m_initialSceneRect = m_scene->sceneRect();
}

MainWindow::~MainWindow() {
    delete m_player;
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
    } else if (event->type() == QEvent::GraphicsSceneMousePress) {
        const auto pressedMouseButton = static_cast<const QGraphicsSceneMouseEvent *>(event)->button();
        if (m_drawing && watched == m_scene && pressedMouseButton == Qt::LeftButton) {
            clickPoint(sceneMousePosition());
        } else if (!m_drawing && watched == m_scene && pressedMouseButton == Qt::RightButton && m_numberOfPoints >= 2) {
            fillFromPoint(sceneMousePosition());
        }
    }
    // https://wiki.qt.io/Smooth_Zoom_In_QGraphicsView
    else if (m_drawing && event->type() == QEvent::GraphicsSceneWheel &&
             QApplication::keyboardModifiers() == Qt::ControlModifier) {
        const auto wheelEvent = static_cast<const QGraphicsSceneWheelEvent *>(event);
        const int numberOfDegrees{ wheelEvent->delta() / 8 };
        const int numberOfSteps{ numberOfDegrees / 15 };
        m_numberOfScheduledScalings += numberOfSteps;
        if (m_numberOfScheduledScalings * numberOfSteps < 0) {
            m_numberOfScheduledScalings = numberOfSteps;
        }
        QTimeLine *animation = new QTimeLine(350, this);
        animation->setUpdateInterval(20);

        connect(animation, SIGNAL(valueChanged(qreal)), this, SLOT(scalingTime(qreal)));
        connect(animation, SIGNAL(finished()), this, SLOT(animationFinished()));
        animation->start();
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

[[maybe_unused]] void MainWindow::openFile() {
    const QString filePath = QFileDialog::getOpenFileName(this, caption, homeDirectory, filter);
    if (!filePath.isEmpty()) {
        const QString extension{ QFileInfo{ filePath }.suffix() };
        if (imageFilters.contains(extension)) {
            openImageFile(filePath);
        } else if (videoFilters.contains(extension)) {
            openVideoFile(filePath);
        }
    }
}

[[maybe_unused]] void MainWindow::saveFile() {
    SaveOpts saveOpts;
    if (!m_saveDialog->exec()) return;
    saveOpts = m_saveDialog->getResult();
    const QString filePath = QFileDialog::getSaveFileName(this, caption, homeDirectory, filter);
    if (filePath.isEmpty()) return;

    QImageWriter writer{ filePath };
    writer.write(imageFromScene(saveOpts));
}

[[maybe_unused]] void MainWindow::closeFile() {
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

void MainWindow::compareJaccard() {
    const QString filePath = QFileDialog::getOpenFileName(this, caption, homeDirectory, filter);
    QImageReader selectedReader(filePath);
    QImage selected = selectedReader.read();
    QImage region = imageFromScene({ true, false, false, false, true });
    const auto data = compareImagesJaccard(selected, region);

    m_compDisplay->prepareDisplay(selected, region, data);
    m_compDisplay->exec();
}

void MainWindow::setupUi() {
    m_ui->setupUi(this);
    new QShortcut(QKeySequence::Close, this, SLOT(close()));
    new QShortcut(Qt::Key_Return, this, SLOT(closePath()));
    new QShortcut(Qt::Key_Space, this, SLOT(pauseAndPlayMovie()));
    setWindowIcon(QIcon{ ":/icons/data/icons/app.png" });
    setWindowTitle(QCoreApplication::applicationName());
}

void MainWindow::setupSceneText() {
    m_image = {};
    m_numberOfPoints = 0;
    m_numberOfScheduledScalings = 0;
    if (m_path && m_path->scene()) {
        m_scene->removeItem(m_path);
        delete m_path;
        m_path = nullptr;
    }
    if (m_selectionItem && m_selectionItem->scene()) {
        m_scene->removeItem(m_selectionItem);
        delete m_selectionItem;
        m_selectionItem = nullptr;
    }
    if (m_video && m_video->scene()) {
        m_scene->removeItem(m_video);
        delete m_video;
        m_video = nullptr;
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
    m_numberOfScheduledScalings = 0;
    if (m_path && m_path->scene()) {
        m_scene->removeItem(m_path);
        delete m_path;
        m_path = nullptr;
    }
    if (m_selectionItem && m_selectionItem->scene()) {
        m_scene->removeItem(m_selectionItem);
        delete m_selectionItem;
        m_selectionItem = nullptr;
    }
    if (m_video && m_video->scene()) {
        m_scene->removeItem(m_video);
        delete m_video;
        m_video = nullptr;
    }
    m_undoStack->clear();
    m_scene->clear();
    m_scene->addPixmap(QPixmap::fromImage(m_image));
    m_scene->setSceneRect(QRectF(0, 0, m_image.width(), m_image.height()));
    m_drawing = true;

    if (!m_selectionItem) {
        m_selectionItem = new SelectionLayerItem(m_painterOptions, m_image.width(), m_image.height());
        m_scene->addItem(m_selectionItem);
    } else {
        m_selectionItem->setSize(m_image.width(), m_image.height());
    }
}

void MainWindow::setupSceneVideo() {
    m_numberOfPoints = 0;
    m_numberOfScheduledScalings = 0;
    if (m_path && m_path->scene()) {
        m_scene->removeItem(m_path);
        delete m_path;
        m_path = nullptr;
    }
    if (m_selectionItem && m_selectionItem->scene()) {
        m_scene->removeItem(m_selectionItem);
        delete m_selectionItem;
        m_selectionItem = nullptr;
    }
    if (m_video && m_video->scene()) {
        m_scene->removeItem(m_video);
        delete m_video;
        m_video = nullptr;
    }
    m_undoStack->clear();
    m_scene->clear();
    m_scene->setSceneRect(m_ui->view->rect());
    m_drawing = false;

    //    if (!m_selectionItem) {
    //        m_selectionItem = new SelectionLayerItem(m_painterOptions, m_image.width(), m_image.height());
    //        m_scene->addItem(m_selectionItem);
    //    } else {
    //        m_selectionItem->setSize(m_image.width(), m_image.height());
    //    }
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

void MainWindow::setupVideoView() {
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
    if (!pointInScene(position)) return;

    m_painterOptions.position = position;
    QUndoCommand *addPointCommand = new AddCommand(m_path ? m_path->getPoints() : QList<QPoint>{}, m_painterOptions,
                                                   m_numberOfPoints, m_scene, final ? &m_drawing : nullptr);
    m_undoStack->push(addPointCommand);
}

void MainWindow::drawPath(const QPoint &position) {
    if (!m_drawing) return;
    QPointF start{};
    // Get the last clicked point in the scene
    for (const auto *item : m_scene->items()) {
        if (qgraphicsitem_cast<const PointItem *>(item)) {
            start = item->pos();
            break;
        }
    }

    if (start.isNull()) return;

    const Point source{ static_cast<int>(start.x()), static_cast<int>(start.y()) };
    const Point destination{ static_cast<int>(position.x()), static_cast<int>(position.y()) };

    if (!pointInScene(source) || !pointInScene(destination)) return;

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

void MainWindow::fillFromPoint(const QPoint &position) {
    if (!pointInScene(position)) return;

    QUndoCommand *command = new RegionSelectCommand(position, pointsFromScene(), m_scene, m_selectionItem);
    m_undoStack->push(command);
}

void MainWindow::closePath() {
    if (!m_drawing || m_numberOfPoints <= 1) return;

    QPoint firstPoint;
    const auto items = m_scene->items();

    for (int i = items.size() - 1; i >= 0; --i) {
        if (qgraphicsitem_cast<const PointItem *>(items[i])) {
            firstPoint = items[i]->pos().toPoint();
            break;
        }
    }

    drawPath(firstPoint);
    clickPoint(firstPoint, true);
}

void MainWindow::scalingTime(qreal x) {
    const double factor{ 1.0 + m_numberOfScheduledScalings / 300.0 };
    m_ui->view->scale(factor, factor);
}

void MainWindow::animationFinished() {
    if (m_numberOfScheduledScalings > 0) {
        --m_numberOfScheduledScalings;
    } else {
        ++m_numberOfScheduledScalings;
    }
}

void MainWindow::pauseAndPlayMovie() {
    auto clear = [this] {
        m_numberOfPoints = 0;
        m_numberOfScheduledScalings = 0;
        if (m_path && m_path->scene()) {
            m_scene->removeItem(m_path);
            delete m_path;
            m_path = nullptr;
        }
        if (m_selectionItem && m_selectionItem->scene()) {
            m_scene->removeItem(m_selectionItem);
            delete m_selectionItem;
            m_selectionItem = nullptr;
        }
        if (m_video && m_video->scene()) {
            m_scene->removeItem(m_video);
        }

        m_undoStack->clear();
        m_scene->clear();

        if (!m_selectionItem) {
            m_selectionItem = new SelectionLayerItem(m_painterOptions, m_image.width(), m_image.height());
            m_scene->addItem(m_selectionItem);
        } else {
            m_selectionItem->setSize(m_image.width(), m_image.height());
        }
    };

    if (m_player) {
        if (m_player->playbackState() == QMediaPlayer::PlaybackState::PlayingState) {
            m_player->pause();
            m_image = m_video->videoSink()->videoFrame().toImage().convertToFormat(QImage::Format_RGBA8888);
            cv::Mat temp(m_image.height(), m_image.width(), CV_8UC4, (cv::Scalar *)m_image.scanLine(0));
            cv::cvtColor(temp, m_imageGray, cv::COLOR_RGBA2GRAY);
            m_graph = std::make_unique<DiagonalGraph<CostFunction>>(CostFunction{}, m_imageGray);

            clear();
            m_scene->addPixmap(QPixmap::fromImage(m_image));
            m_scene->setSceneRect(QRectF(0, 0, m_image.width(), m_image.height()));
            m_drawing = true;
        } else {
            m_player->play();

            clear();
            m_scene->addItem(m_video);
        }
    }
}

void MainWindow::openImageFile(const QString &filePath) {
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

void MainWindow::openVideoFile(const QString &filePath) {
    delete m_player;
    m_player = new QMediaPlayer;
    m_video = new QGraphicsVideoItem;
    m_video->setSize(m_ui->view->size());
    setupSceneVideo();
    setupVideoView();
    m_player->setVideoOutput(m_video);
    m_scene->addItem(m_video);
    m_player->setSource(QUrl{ filePath });
    m_player->play();
    const QFileInfo info{ filePath };
    setWindowTitle(QCoreApplication::applicationName() + " - " + info.fileName());
    setupActions(true);
}

bool MainWindow::pointInScene(Point point) const {
    return point.x >= 0 && point.y >= 0 && point.x < m_scene->width() && point.y < m_scene->height();
}

bool MainWindow::pointInScene(QPoint point) const {
    return pointInScene(Point{ point.x(), point.y() });
}

std::unordered_set<QPoint> MainWindow::pointsFromScene() const {
    std::unordered_set<QPoint> points;

    const PathItem *pathItem;
    for (const auto *item : m_scene->items()) {
        if (pathItem = qgraphicsitem_cast<const PathItem *>(item); pathItem != nullptr) {
            for (const auto &p : pathItem->getPoints()) {
                points.insert(p);
            }
        }
    }

    return points;
}

QImage MainWindow::imageFromScene(SaveOpts opts) {
    static const QColor empty{ 0, 0, 0, 0 };
    const int w = m_image.width(), h = m_image.height();
    QColor eraseColor = empty;
    QImage result = m_image;
    QPainter painter(&result);

    if (opts.binarize) {
        eraseColor = Qt::black;
        result.fill(Qt::white);
    }

    if (opts.inside != opts.outside) {
        const auto &selection = m_selectionItem->selected();
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                if (selection[w * y + x] == opts.outside) result.setPixelColor(x, y, eraseColor);
    } else if (!opts.inside) {
        result.fill(eraseColor);
    }

    for (auto *item : m_scene->items()) {
        if ((!opts.path || !qgraphicsitem_cast<const PathItem *>(item)) &&
            (!opts.points || !qgraphicsitem_cast<const PointItem *>(item))) {
            item->hide();
        }
    }

    m_scene->render(&painter);
    for (auto *item : m_scene->items())
        item->show();

    return result;
}
