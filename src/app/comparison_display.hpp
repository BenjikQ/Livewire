#ifndef COMPARISON_DISPLAY_HPP
#define COMPARISON_DISPLAY_HPP

#include <vector>

#include <QDialog>
#include <QFrame>

#include "image_label.hpp"
#include "imgprc_helpers.hpp"

namespace Ui {
class ComparisonDisplay;
}

class ComparisonDisplay : public QDialog {
    Q_OBJECT

public:
    explicit ComparisonDisplay(QWidget *parent = nullptr);
    ~ComparisonDisplay();

    bool prepareDisplay(const QImage &img1, const QImage &img2, const JaccardData &data);

private:
    Ui::ComparisonDisplay *m_ui;
    ImageLabel *m_loadedLabel, *m_selectionLabel, *m_compLabel;

    ImageLabel *createImageLabel();
};

#endif  // COMPARISON_DISPLAY_HPP
