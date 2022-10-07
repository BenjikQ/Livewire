#include "comparison_display.hpp"

#include <QImage>
#include <QImageReader>

#include "ui_comparison_display.h"

constexpr int LABEL_MIN_SIZE = 200;
constexpr int LABEL_MAX_SIZE = 500;

ComparisonDisplay::ComparisonDisplay(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ComparisonDisplay) {
    m_ui->setupUi(this);
    m_ui->compBox->layout()->addWidget(m_compLabel = createImageLabel());
    m_ui->currentBox->layout()->addWidget(m_selectionLabel = createImageLabel());
    m_ui->loadedBox->layout()->addWidget(m_loadedLabel = createImageLabel());
}

ComparisonDisplay::~ComparisonDisplay() {
    delete m_ui;
}

bool ComparisonDisplay::prepareDisplay(const QImage &img1, const QImage &img2, const JaccardData &data) {
    std::string temp;

    m_selectionLabel->setPixmap(QPixmap::fromImage(img1));
    m_loadedLabel->setPixmap(QPixmap::fromImage(img2));
    m_compLabel->setPixmap(QPixmap::fromImage(data.compImage));
    m_ui->conjAreaValue->setText((temp = std::to_string(data.conj)).c_str());
    m_ui->disjAreaValue->setText((temp = std::to_string(data.disj)).c_str());
    m_ui->JaccardValue->setText((temp = std::to_string(data.coeff)).c_str());

    return true;
}

ImageLabel *ComparisonDisplay::createImageLabel() {
    ImageLabel *label = new ImageLabel(this);
    label->setMinimumSize({ LABEL_MIN_SIZE, LABEL_MIN_SIZE });
    label->setMaximumSize({ LABEL_MAX_SIZE, LABEL_MAX_SIZE });
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    return label;
}
