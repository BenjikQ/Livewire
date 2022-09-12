#include "comparison_display.hpp"

#include <QImage>
#include <QImageReader>

#include "ui_comparison_display.h"

ComparisonDisplay::ComparisonDisplay(QWidget *parent) :
    QDialog(parent), ui(new Ui::ComparisonDisplay) {
    ui->setupUi(this);
}

ComparisonDisplay::~ComparisonDisplay() { delete ui; }

bool ComparisonDisplay::prepareDisplay(const QImage &img1, const QImage &img2,
                                       const JaccardData &data) {
    std::string temp;

    ui->selectionLabel->setPixmap(QPixmap::fromImage(img1));
    ui->loadedLabel->setPixmap(QPixmap::fromImage(img2));
    ui->compLabel->setPixmap(QPixmap::fromImage(data.compImage));
    ui->conjAreaValue->setText((temp = std::to_string(data.conj)).c_str());
    ui->disjAreaValue->setText((temp = std::to_string(data.disj)).c_str());
    ui->JaccardValue->setText((temp = std::to_string(data.coeff)).c_str());

    return true;
}
