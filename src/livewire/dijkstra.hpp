#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include <algorithm>

#include "typedefs.hpp"


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

template <typename T>
std::vector<Point> dijkstra(const T &graph, const Point &source,
                            const Point &dest) {
    Array2D<double> dist{ graph.w(), graph.h(), INF };
    Array2D<uint8_t> visited{ graph.w(), graph.h(), false };
    Array2D<Point> prev{ graph.w(), graph.h(), { -1, -1 } };
    Point current = source;
    auto remaining = visited.data.size() - 1;

    dist[source] = 0;
    visited[source] = true;

    while (remaining > 0 && !visited[dest]) {
        visited[current] = true;
        for (Dir d{}; d != Dir::COUNT; ++d) {
            const Point p = current + d;
            if (p.x < 0 || p.y < 0 || p.x >= graph.w() || p.y >= graph.h() ||
                visited[p])
                continue;
            double newDist = INF;
            if ((newDist = dist[current] + graph.distance(current, d)) <
                dist[p]) {
                dist[p] = newDist;
                prev[p] = current;
            }
        }
        current = nextOrigin(dist, visited);
    }

    return compilePath(prev, dest);
}

#endif  // DIJKSTRA_HPP
