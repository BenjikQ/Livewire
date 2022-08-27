#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include <functional>
#include <queue>
#include <ranges>
#include <type_traits>
#include <vector>

#include "typedefs.hpp"

namespace detail {

struct Node {
    Point position;
    double priority;
};

constexpr bool operator<(const Node &l, const Node &r) {
    return l.priority > r.priority;
}

template <class Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
    return static_cast<std::underlying_type_t<Enum>>(e);
}

constexpr auto enum_range = [](auto front, auto back) {
    return std::views::iota(to_underlying(front), to_underlying(back) + 1) |
           std::views::transform([](auto e) { return decltype(front)(e); });
};

}  // namespace detail

std::vector<Point> compilePath(const Array2D<Point> &prev, const Point &dest);

template <typename Graph>
std::vector<Point> dijkstra(const Graph &graph, const Point &source,
                            const Point &dest) {
    Array2D<double> dist{ graph.w(), graph.h(), INF };
    Array2D<Point> prev{ graph.w(), graph.h(), { -1, -1 } };
    dist[source] = 0;

    std::priority_queue<detail::Node> queue;
    queue.push({ source, 0 });

    while (!queue.empty()) {
        const auto [position, priority] = queue.top();
        queue.pop();
        if (position == dest) break;
        if (priority == dist[position]) {
            for (const auto dir :
                 detail::enum_range(Dir::RIGHT, Dir::BOTTOMRIGHT)) {
                const auto v = position + dir;
                if (v.x >= 0 && v.y >= 0 && v.x < graph.w() &&
                    v.y < graph.h()) {
                    const auto alt =
                        dist[position] + graph.distance(position, dir);
                    if (alt < dist[v] && dist[position] != INF) {
                        dist[v] = alt;
                        prev[v] = position;
                        queue.push({ v, alt });
                    }
                }
            }
        }
    }

    return compilePath(prev, dest);
}

#endif  // DIJKSTRA_HPP
