#include "dijkstra.hpp"

Point nextOrigin(const Array2D<double> &dist, const Array2D<uint8_t> &visited) {
    double mindist = INF;
    int64_t minind = -1;
    for (int64_t i = 0; i < int64_t(dist.data.size()); ++i) {
        if (!visited.data[i] && dist.data[i] < mindist) {
            mindist = dist.data[i];
            minind = i;
        }
    }
    return { minind % dist.w, minind / dist.w };
}

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
