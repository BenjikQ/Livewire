#ifndef PRESAVE_DIALOG_HPP
#define PRESAVE_DIALOG_HPP

#include <QDialog>

#include "typedefs.hpp"

namespace Ui {
class PresaveDialog;
}

class PresaveDialog : public QDialog {
    Q_OBJECT

public:
    explicit PresaveDialog(QWidget *parent = nullptr);
    ~PresaveDialog();

    SaveOptions getResult() const;

private:
    Ui::PresaveDialog *ui;
};

#endif  // PRESAVE_DIALOG_HPP
