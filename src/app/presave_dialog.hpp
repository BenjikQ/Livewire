#ifndef PRESAVE_DIALOG_HPP
#define PRESAVE_DIALOG_HPP

#include <QDialog>

namespace Ui {
class PresaveDialog;
}

struct SaveOpts {
    uint8_t inside : 1, outside : 1, path : 1, points : 1, binarize : 1;
};

class PresaveDialog : public QDialog {
    Q_OBJECT

public:
    explicit PresaveDialog(QWidget *parent = nullptr);
    ~PresaveDialog();

    SaveOpts getResult() const;

private:
    Ui::PresaveDialog *ui;
};

#endif  // PRESAVE_DIALOG_HPP
