#ifndef REGIONS_HPP
#define REGIONS_HPP

#include <unordered_set>

#include "typedefs.hpp"

PathData getPathInfo(const std::vector<Point> &path);
Array2D<uint8_t> floodFillRegion(const PathData &pi, Point origin);

#endif  // REGIONS_HPP
