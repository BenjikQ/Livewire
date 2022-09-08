#include "image_viewer.hpp"

#include <QFileDialog>
#include <QMap>
#include <QShortcut>
#include <QStandardPaths>

#include "paint_area.hpp"
#include "ui_image_viewer.h"

//static const QMap<QString, QColor> colors{
//    { "black", QColorConstants::Black },
//    { "blue", QColorConstants::Blue },
//    { "cyan", QColorConstants::Cyan },
//    { "green", QColorConstants::Green },
//    { "gray", QColorConstants::Gray },
//    { "magenta", QColorConstants::Magenta },
//    { "orange", QColorConstants::Svg::orange },
//    { "red", QColorConstants::Red },
//    { "white", QColorConstants::White },
//    { "yellow", QColorConstants::Yellow },
//};

ImageViewer::ImageViewer(QWidget *parent) :
    QMainWindow(parent),
    paintArea(new PaintArea(this)),
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
//        const QString styleSheet = "background: " + colorButton->objectName();
//        const QColor color = colors[colorButton->objectName()];
//        qDebug() << styleSheet << color;
//        connect(colorButton, &QPushButton::clicked, this, [&]() {
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
    const QString filePath =
        QFileDialog::getSaveFileName(this, tr("Open Image"), homeDirectory,
                                     tr("Image Files (*.png *.jpg *.bmp)"));
    if (!filePath.isEmpty()) paintArea->save(filePath);
}

void ImageViewer::closePath() { paintArea->finalizePath(); }

void ImageViewer::undo() { paintArea->undo(); }
