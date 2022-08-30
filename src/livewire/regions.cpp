#include "regions.hpp"

PathInfo getPathInfo(const std::vector<Point> &path) {
    PathInfo info{ Point::maxValue(), -Point::maxValue(), {} };

    for (const auto &point : path) {
        info.topLeft.x = std::min(info.topLeft.x, point.x);
        info.topLeft.y = std::min(info.topLeft.y, point.y);
        info.bottomRight.x = std::max(info.bottomRight.x, point.x);
        info.bottomRight.y = std::max(info.bottomRight.y, point.y);

        info.pts.insert(point);
    }

    return info;
}
