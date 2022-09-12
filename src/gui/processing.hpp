#ifndef PROCESSING_HPP
#define PROCESSING_HPP

#include <QImage>

#include "typedefs.hpp"

struct JaccardData {
    QImage compImage;
    double coeff = NaN;
    int conj = 0, disj = 0, conc = 0, disc = 0;
    bool valid = false;
};

JaccardData compareImagesJaccard(const QImage &im1, const QImage &im2);


#endif  // PROCESSING_HPP
