#ifndef COMPARISON_DISPLAY_HPP
#define COMPARISON_DISPLAY_HPP

#include <QDialog>
#include <QFrame>
#include <vector>

#include "image_label.hpp"
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
    ImageLabel *loadedLabel, *selectionLabel, *compLabel;

    ImageLabel *createImageLabel();
};

#endif  // COMPARISON_DISPLAY_HPP
