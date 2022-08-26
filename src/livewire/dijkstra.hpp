#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include <boost/heap/fibonacci_heap.hpp>
#include <functional>
#include <ranges>
#include <type_traits>
#include <vector>

#include "typedefs.hpp"

namespace detail {

struct Node {
    Point positon;
    double priority;
};

struct CompareNodes {
    bool operator()(const Node &l, const Node &r) const {
        return l.priority > r.priority;
    }
};

class Queue {
    using Heap =
        boost::heap::fibonacci_heap<Node, boost::heap::compare<CompareNodes>>;

    std::vector<std::pair<Point, Heap::handle_type>> handles;
    Heap heap;

public:
    Node extract() {
        const auto top = heap.top();
        handles.erase(std::remove_if(handles.begin(), handles.end(),
                                     [&top](const auto &h) {
                                         return h.first == top.positon;
                                     }),
                      handles.end());
        heap.pop();
        return top;
    }

    void add(const Point &p, double priority) {
        const auto handle = heap.push({ p, priority });
        handles.push_back({ p, handle });
    }

    void decrease(const Point &p, double priority) {
        const auto handle =
            std::find_if(handles.begin(), handles.end(),
                         [&p](const auto &h) { return h.first == p; });
        heap.decrease(handle->second, { p, priority });
    }

    bool empty() const { return heap.empty(); }

    bool contains(const Point &p) const {
        return std::find_if(handles.begin(), handles.end(),
                            [&p](const auto &h) { return h.first == p; }) !=
               handles.end();
    }
};

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

    detail::Queue queue;
    queue.add(source, dist[source]);

    while (!queue.empty()) {
        const auto u = queue.extract();
        if (u.positon == dest) break;
        for (const auto dir :
             detail::enum_range(Dir::RIGHT, Dir::BOTTOMRIGHT)) {
            const auto v = u.positon + dir;
            if (v.x >= 0 && v.y >= 0 && v.x < graph.w() && v.y < graph.h()) {
                const auto alt =
                    dist[u.positon] + graph.distance(u.positon, dir);
                if (alt < dist[v] && dist[u.positon] != INF) {
                    dist[v] = alt;
                    prev[v] = u.positon;
                    if (queue.contains(v)) {
                        queue.decrease(v, alt);
                    } else {
                        queue.add(v, alt);
                    }
                }
            }
        }
    }

    return compilePath(prev, dest);
}

#endif  // DIJKSTRA_HPP
