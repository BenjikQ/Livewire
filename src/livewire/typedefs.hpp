#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <cstdint>
#include <iostream>
#include <limits>
#include <unordered_set>
#include <vector>

constexpr double INF = std::numeric_limits<double>::infinity();

enum Dir {
    RIGHT,
    TOPRIGHT,
    TOP,
    TOPLEFT,
    LEFT,
    BOTTOMLEFT,
    BOTTOM,
    BOTTOMRIGHT,
    COUNT
};

constexpr Dir operator+(Dir d, int n) { return Dir(int(d) + n); }
constexpr Dir operator-(Dir d, int n) { return d + (-n); }
constexpr Dir operator-(Dir d) { return d + 4; }
constexpr Dir operator++(Dir &d) { return d = d + 1; }
constexpr Dir operator+=(Dir &d, int x) { return d = d + x; }

struct Point {
    int64_t x, y;

    constexpr Point() = default;
    constexpr Point(int64_t x, int64_t y) : x(x), y(y) {}

    constexpr Point operator+(Dir d) const {
        switch (d) {
        case Dir::RIGHT: return { x + 1, y };
        case Dir::TOPRIGHT: return { x + 1, y - 1 };
        case Dir::TOP: return { x, y - 1 };
        case Dir::TOPLEFT: return { x - 1, y - 1 };
        case Dir::LEFT: return { x - 1, y };
        case Dir::BOTTOMLEFT: return { x - 1, y + 1 };
        case Dir::BOTTOM: return { x, y + 1 };
        case Dir::BOTTOMRIGHT: return { x + 1, y + 1 };
        default: return { x, y };
        }
    }

    constexpr Point operator-(Dir d) const { return operator+(-d); }

    constexpr Point operator-() const { return { -x, -y }; }

    constexpr bool operator==(const Point &other) const {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const Point &other) const {
        return !(*this == other);
    }

    constexpr static Point maxValue() {
        constexpr int64_t maxval = std::numeric_limits<int64_t>::max();
        return { maxval, maxval };
    }

    template <typename T>
    static Point from(const T &val) {
        return { val.x(), val.y() };
    }
};

std::ostream &operator<<(std::ostream &ost, const Point &p);

namespace std {

template <>
struct hash<Point> {
    std::size_t operator()(const Point &k) const {
        return (hash<int64_t>()(k.x) ^ (hash<int64_t>()(k.y) << 1));
    }
};

}  // namespace std

struct PathData {
    Point topLeft, bottomRight;
    std::unordered_set<Point> pts;
};

struct Node {
    Point p{}, prev{ -1, -1 };
    double dist = INF;
    bool visited = false;

    bool operator<(const Node &other) const { return other.dist < dist; }
};

template <typename T>
struct Array2D {
    int64_t w, h;
    std::vector<T> data;

    Array2D(int64_t w, int64_t h, T val = {}) : w(w), h(h), data(w * h, val) {}

    T operator[](const Point &p) const { return data[p.y * w + p.x]; }
    T &operator[](const Point &p) { return data[p.y * w + p.x]; }

    void print() const {
        for (int64_t y = 0; y < h; ++y) {
            for (int64_t x = 0; x < w; ++x)
                std::cout << data[y * w + x] << ' ';
            std::cout << '\n';
        }
    }
};

struct SaveOptions {
    bool saveInside, saveOutside, savePath, savePoints;
};

#endif  // TYPEDEFS_HPP
