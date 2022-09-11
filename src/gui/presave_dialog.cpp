#include "presave_dialog.hpp"

#include <iostream>

#include "typedefs.hpp"
#include "ui_presave_dialog.h"

PresaveDialog::PresaveDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::PresaveDialog) {
    ui->setupUi(this);
}

PresaveDialog::~PresaveDialog() { delete ui; }

SaveOptions PresaveDialog::getResult() const {
    SaveOptions opts{};

    opts.saveInside = ui->saveInside->isChecked();
    opts.saveOutside = ui->saveOutside->isChecked();
    opts.savePath = ui->savePath->isChecked();
    opts.savePoints = ui->savePoints->isChecked();
    opts.saveTextFile = ui->saveTextFile->isChecked();
    opts.binarize = ui->binarize->isChecked();

    return opts;
}
