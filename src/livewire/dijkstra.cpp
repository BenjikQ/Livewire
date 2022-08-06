#include "dijkstra.hpp"

std::vector<Point> compilePath(const Array2D<Point> &prev, const Point &dest) {
    std::vector<Point> path{};
    Point cur = dest;
    while (prev[cur].x >= 0) {
        path.push_back(cur);
        cur = prev[cur];
    }
    path.push_back(cur);
    std::reverse(path.begin(), path.end());
    return path;
}
