#include "image_viewer.hpp"

#include <QFileDialog>
#include <QShortcut>
#include <QStandardPaths>

#include "paint_area.hpp"
#include "presave_dialog.hpp"
#include "ui_image_viewer.h"

ImageViewer::ImageViewer(QWidget *parent) :
    QMainWindow(parent),
    paintArea(new PaintArea(this)),
    presaveDialog(new PresaveDialog(this)),
    ui(new Ui::ImageViewer) {
    ui->setupUi(this);
    new QShortcut(Qt::CTRL | Qt::Key_W, this, SLOT(close()));
    new QShortcut(Qt::Key_Return, this, SLOT(closePath()));
    QSize size = this->size();
    statusBar()->showMessage(QString::number(size.width()) + " x " +
                             QString::number(size.height()) + " piks.");
    ui->scrollArea->setVisible(false);
    ui->scrollArea->setWidget(paintArea);
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

void ImageViewer::closePath() { paintArea->finalizePath(); }
