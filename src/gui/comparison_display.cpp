#include "comparison_display.hpp"

#include <QImage>
#include <QImageReader>

#include "ui_comparison_display.h"

ComparisonDisplay::ComparisonDisplay(QWidget *parent) :
    QDialog(parent), ui(new Ui::ComparisonDisplay) {
    ui->setupUi(this);
    ui->compBox->layout()->addWidget(compLabel = createImageLabel());
    ui->currentBox->layout()->addWidget(selectionLabel = createImageLabel());
    ui->loadedBox->layout()->addWidget(loadedLabel = createImageLabel());
}

ComparisonDisplay::~ComparisonDisplay() { delete ui; }

bool ComparisonDisplay::prepareDisplay(const QImage &img1, const QImage &img2,
                                       const JaccardData &data) {
    std::string temp;

    selectionLabel->setPixmap(QPixmap::fromImage(img1));
    loadedLabel->setPixmap(QPixmap::fromImage(img2));
    compLabel->setPixmap(QPixmap::fromImage(data.compImage));
    ui->conjAreaValue->setText((temp = std::to_string(data.conj)).c_str());
    ui->disjAreaValue->setText((temp = std::to_string(data.disj)).c_str());
    ui->JaccardValue->setText((temp = std::to_string(data.coeff)).c_str());

    return true;
}

ImageLabel *ComparisonDisplay::createImageLabel() {
    ImageLabel *label = new ImageLabel(this);
    label->setMinimumSize({ 200, 200 });
    label->setMaximumSize({ 500, 500 });
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    return label;
}
