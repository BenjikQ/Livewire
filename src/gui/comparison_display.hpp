#ifndef COMPARISON_DISPLAY_HPP
#define COMPARISON_DISPLAY_HPP

#include <QDialog>
#include <QFrame>
#include <vector>

#include "processing.hpp"

namespace Ui {
class ComparisonDisplay;
}

class ComparisonDisplay : public QDialog {
    Q_OBJECT

public:
    explicit ComparisonDisplay(QWidget *parent = nullptr);
    ~ComparisonDisplay();

    bool prepareDisplay(const QImage &img1, const QImage &img2,
                        const JaccardData &data);

private:
    Ui::ComparisonDisplay *ui;

    static void setFrameBg(QFrame *frame, const QImage &img);
};

#endif  // COMPARISON_DISPLAY_HPP
