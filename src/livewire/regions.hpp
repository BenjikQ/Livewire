#ifndef REGIONS_HPP
#define REGIONS_HPP

#include <unordered_set>

#include "typedefs.hpp"

struct PathInfo {
    Point topLeft, bottomRight;
    std::unordered_set<Point> pts;
};

PathInfo getPathInfo(const std::vector<Point> &path);
Array2D<uint8_t> floodFillRegion(const PathInfo &pi, Point origin);

#endif  // REGIONS_HPP
