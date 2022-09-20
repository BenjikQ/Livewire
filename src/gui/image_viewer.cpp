#include "image_viewer.hpp"

#include <QFileDialog>
#include <QImageReader>
#include <QMap>
#include <QShortcut>
#include <QStandardPaths>

#include "comparison_display.hpp"
#include "paint_area.hpp"
#include "presave_dialog.hpp"
#include "processing.hpp"
#include "ui_image_viewer.h"

// static const QMap<QString, QColor> colors{
//     { "black", QColorConstants::Black },
//     { "blue", QColorConstants::Blue },
//     { "cyan", QColorConstants::Cyan },
//     { "green", QColorConstants::Green },
//     { "gray", QColorConstants::Gray },
//     { "magenta", QColorConstants::Magenta },
//     { "orange", QColorConstants::Svg::orange },
//     { "red", QColorConstants::Red },
//     { "white", QColorConstants::White },
//     { "yellow", QColorConstants::Yellow },
// };

ImageViewer::ImageViewer(QWidget *parent) :
    QMainWindow(parent),
    paintArea(new PaintArea(this)),
    presaveDialog(new PresaveDialog(this)),
    compDisplay(new ComparisonDisplay(this)),
    ui(new Ui::ImageViewer) {
    ui->setupUi(this);
    new QShortcut(Qt::CTRL | Qt::Key_W, this, SLOT(close()));
    new QShortcut(Qt::Key_Return, this, SLOT(closePath()));
    QSize size = this->size();
    statusBar()->showMessage(QString::number(size.width()) + " x " +
                             QString::number(size.height()) + " piks.");
    ui->scrollArea->setVisible(false);
    ui->scrollArea->setWidget(paintArea);

    // Didn't work in ui designer
    connect(ui->horizontalSlider, &QSlider::valueChanged, this, [&]() {
        ui->penWidthLabel->setNum(ui->horizontalSlider->value());
        paintArea->setPenWidth(ui->horizontalSlider->value());
    });

    // Setup color picker
    //    connect(ui->black, &QPushButton::clicked, this, [&]() {
    //        ui->color->setStyleSheet("background: black");
    //        paintArea->setPenColor(QColorConstants::Black);
    //        qDebug() << ui->black->objectName();
    //    });

    connect(ui->black, &QPushButton::clicked, this, [&]() {
        ui->color->setStyleSheet("background: black");
        paintArea->setPenColor(QColorConstants::Black);
    });

    connect(ui->blue, &QPushButton::clicked, this, [&]() {
        ui->color->setStyleSheet("background: blue");
        paintArea->setPenColor(QColorConstants::Blue);
    });

    connect(ui->cyan, &QPushButton::clicked, this, [&]() {
        ui->color->setStyleSheet("background: cyan");
        paintArea->setPenColor(QColorConstants::Cyan);
    });

    connect(ui->green, &QPushButton::clicked, this, [&]() {
        ui->color->setStyleSheet("background: green");
        paintArea->setPenColor(QColorConstants::Green);
    });

    connect(ui->gray, &QPushButton::clicked, this, [&]() {
        ui->color->setStyleSheet("background: gray");
        paintArea->setPenColor(QColorConstants::Gray);
    });

    connect(ui->magenta, &QPushButton::clicked, this, [&]() {
        ui->color->setStyleSheet("background: magenta");
        paintArea->setPenColor(QColorConstants::Magenta);
    });

    connect(ui->orange, &QPushButton::clicked, this, [&]() {
        ui->color->setStyleSheet("background: orange");
        paintArea->setPenColor(QColorConstants::Svg::orange);
    });

    connect(ui->red, &QPushButton::clicked, this, [&]() {
        ui->color->setStyleSheet("background: red");
        paintArea->setPenColor(QColorConstants::Red);
    });

    connect(ui->white, &QPushButton::clicked, this, [&]() {
        ui->color->setStyleSheet("background: white");
        paintArea->setPenColor(QColorConstants::White);
    });

    connect(ui->yellow, &QPushButton::clicked, this, [&]() {
        ui->color->setStyleSheet("background: yellow");
        paintArea->setPenColor(QColorConstants::Yellow);
    });

    //    const QList<QPushButton *> &colorPicker =
    //        ui->groupBox2->findChildren<QPushButton *>();
    //    for (const auto &colorButton : colorPicker) {
    //        const QString styleSheet = "background: " +
    //        colorButton->objectName(); const QColor color =
    //        colors[colorButton->objectName()]; qDebug() << styleSheet <<
    //        color; connect(colorButton, &QPushButton::clicked, this, [&]() {
    //            qDebug() << styleSheet << color;
    //            ui->color->setStyleSheet(styleSheet);
    //            paintArea->setPenColor(colors[colorButton->objectName()]);
    //        });
    //    }
}

ImageViewer::~ImageViewer() { delete ui; }

void ImageViewer::open() {
    static const QStringList homePath =
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory =
        homePath.first().split(QDir::separator()).last();
    const QString filePath =
        QFileDialog::getOpenFileName(this, tr("Open Image"), homeDirectory,
                                     tr("Image Files (*.png *.jpg *.bmp)"));
    const QString fileName = QFileInfo(filePath).fileName();
    setWindowTitle(fileName + " - Livewire");
    if (!filePath.isEmpty()) {
        paintArea->open(filePath);
        ui->scrollArea->setVisible(true);
        ui->horizontalLayout->removeItem(ui->horizontalSpacer);
    }
}

void ImageViewer::save() {
    static const QStringList homePath =
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory =
        homePath.first().split(QDir::separator()).last();

    SaveOptions opts{};
    if (presaveDialog->exec())
        opts = presaveDialog->getResult();
    else
        return;

    const QString filePath =
        QFileDialog::getSaveFileName(this, tr("Open Image"), homeDirectory,
                                     tr("Image Files (*.png *.jpg *.bmp)"));

    if (!filePath.isEmpty()) paintArea->save(filePath, opts);
}

void ImageViewer::saveOutlines() {
    static const QStringList homePath =
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory =
        homePath.first().split(QDir::separator()).last();
    const QString filePath = QFileDialog::getSaveFileName(
        this, tr("Open Image"), homeDirectory, tr("Text files (*.txt)"));
    if (!filePath.isEmpty()) {
        paintArea->saveOutlines(filePath);
    }
}

void ImageViewer::loadOutlines() {
    static const QStringList homePath =
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory =
        homePath.first().split(QDir::separator()).last();
    const QString filePath =
        QFileDialog::getOpenFileName(this, tr("Open outlines file"),
                                     homeDirectory, tr("Text files (*.txt)"));
    if (!filePath.isEmpty()) {
        paintArea->clear();
        paintArea->loadOutlines(filePath);
    }
}

void ImageViewer::loadOutlinesWithColors() {
    static const QStringList homePath =
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory =
        homePath.first().split(QDir::separator()).last();
    const QString filePath =
        QFileDialog::getOpenFileName(this, tr("Open outlines with colors file"),
                                     homeDirectory, tr("Text files (*.txt)"));
    if (!filePath.isEmpty()) {
        paintArea->clear();
        paintArea->loadOutlinesWithColors(filePath);
    }
}

void ImageViewer::closePath() { paintArea->finalizePath(); }

void ImageViewer::undo() { paintArea->undo(); }

void ImageViewer::compareJaccard() {
    static const QStringList homePath =
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    static const QString homeDirectory =
        homePath.first().split(QDir::separator()).last();
    const QString filePath = QFileDialog::getOpenFileName(
        this, tr("Select image to compare with"), homeDirectory,
        tr("Image Files (*.png *.jpg *.bmp)"));
    if (filePath.isEmpty()) return;

    QImageReader selectedReader(filePath);
    QImage selected = selectedReader.read();
    QImage region =
        paintArea->composeImage({ true, false, false, false, true });
    const auto data = compareImagesJaccard(selected, region);

    compDisplay->prepareDisplay(selected, region, data);
    compDisplay->exec();
}

void ImageViewer::zoomIn() { paintArea->zoomIn(); }

void ImageViewer::zoomOut() { paintArea->zoomOut(); }

void ImageViewer::setDrawingMode() { paintArea->setDrawingMode(); }

void ImageViewer::setEditingMode() { paintArea->setEditingMode(); }
