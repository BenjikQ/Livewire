#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include <algorithm>
#include <queue>

#include "typedefs.hpp"

Point nextOrigin(const Array2D<double> &dist, const Array2D<uint8_t> &visited);
std::vector<Point> compilePath(const Array2D<Point> &prev, const Point &dest);

struct Priority {
    Point p;
    double priority;

    [[nodiscard]] friend constexpr auto operator<(
        const Priority &lhs, const Priority &rhs) noexcept {
        return lhs.priority > rhs.priority;
    }
};

template <class T, class Container = std::vector<T>,
          class Compare = std::less<typename Container::value_type>>
class MyQueue : public std::priority_queue<T, Container, Compare> {
public:
    typedef typename std::priority_queue<
        T, Container, Compare>::container_type::const_iterator const_iterator;

    const_iterator cbegin() const noexcept { return this->c.cbegin(); }
    const_iterator cend() const noexcept { return this->c.cend(); }
};

template <typename T>
std::vector<Point> dijkstra(const T &graph, const Point &source,
                            const Point &dest) {
    Array2D<double> dist{ graph.w(), graph.h(), INF };
    Array2D<Point> prev{ graph.w(), graph.h(), { -1, -1 } };
    dist[source] = 0;

    MyQueue<Priority> Q;
    Q.push({ source, dist[source] });

    while (!Q.empty()) {
        const auto u = Q.top();
        Q.pop();
        if (u.priority == dist[u.p]) {
            for (Dir d{}; d != Dir::COUNT; ++d) {
                const auto v = u.p + d;
                if (v.x >= 0 && v.y >= 0 && v.x < graph.w() &&
                    v.y < graph.h()) {
                    const auto alt = dist[u.p] + graph.distance(u.p, d);
                    if (alt < dist[v] && dist[u.p] != INF) {
                        dist[v] = alt;
                        prev[v] = u.p;
                        Q.push({ v, alt });
                    }
                }
            }
        }
    }

    return compilePath(prev, dest);
}

#endif  // DIJKSTRA_HPP
