#ifndef IMGPRC_HELPERS_HPP
#define IMGPRC_HELPERS_HPP

#include <limits>
#include <unordered_set>
#include <vector>

#include <QGraphicsScene>
#include <QImage>
#include <QImageWriter>
#include <QPoint>

namespace std {

template <>
struct hash<QPoint> {
    std::size_t operator()(const QPoint &k) const { return (hash<int>()(k.x()) ^ (hash<int>()(k.y()) << 1)); }
};

}  // namespace std

struct JaccardData {
    QImage compImage;
    double coeff = std::numeric_limits<double>::quiet_NaN();
    int conj = 0, disj = 0, conc = 0, disc = 0;
    bool valid = false;
};

std::vector<bool> floodFill(const std::vector<bool> &region, const std::unordered_set<QPoint> &outline, QPoint origin,
                            int w, int h);

JaccardData compareImagesJaccard(const QImage &im1, const QImage &im2);

#endif  // IMGPRC_HELPERS_HPP
