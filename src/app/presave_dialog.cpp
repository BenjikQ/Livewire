#include "presave_dialog.hpp"

#include "ui_presave_dialog.h"

PresaveDialog::PresaveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PresaveDialog) {
    ui->setupUi(this);
}

PresaveDialog::~PresaveDialog() {
    delete ui;
}

SaveOpts PresaveDialog::getResult() const {
    SaveOpts opts;

    opts.inside = ui->saveInside->isChecked();
    opts.outside = ui->saveOutside->isChecked();
    opts.path = ui->savePath->isChecked();
    opts.points = ui->savePoints->isChecked();
    opts.binarize = ui->binarize->isChecked();

    return opts;
}
