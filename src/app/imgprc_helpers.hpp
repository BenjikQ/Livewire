#ifndef IMGPRC_HELPERS_HPP
#define IMGPRC_HELPERS_HPP

#include <unordered_set>
#include <vector>

#include <QPoint>

namespace std {

template <>
struct hash<QPoint> {
    std::size_t operator()(const QPoint &k) const { return (hash<int>()(k.x()) ^ (hash<int>()(k.y()) << 1)); }
};

}  // namespace std

std::vector<bool> floodFill(const std::vector<bool> &region, const std::unordered_set<QPoint> &outline, QPoint origin,
                            int w, int h);

#endif  // IMGPRC_HELPERS_HPP
