#pragma once
#include <cstdint>
#include <limits>
#include <iostream>
#include <vector>

constexpr double INF = std::numeric_limits<double>::infinity();

enum Dir : uint8_t {
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

constexpr Dir operator+(Dir d, int n) {
    return Dir(int(d) + n);
}
constexpr Dir operator-(Dir d, int n) { return d + (-n); }
constexpr Dir operator-(Dir d) { return d + 4; }
constexpr Dir operator++(Dir &d) { return d = d + 1; }

struct Point {
    int64_t x, y;

    constexpr Point operator+(Dir d) const {
        switch (d) {
        case Dir::RIGHT:	   return { x + 1, y };
        case Dir::TOPRIGHT:	   return { x + 1, y - 1 };
        case Dir::TOP:		   return { x, y - 1 };
        case Dir::TOPLEFT:	   return { x - 1, y - 1 };
        case Dir::LEFT:		   return { x - 1, y };
        case Dir::BOTTOMLEFT:  return { x - 1, y + 1 };
        case Dir::BOTTOM:	   return { x, y + 1 };
        case Dir::BOTTOMRIGHT: return { x + 1, y + 1 };
        default:			   return { x, y };
        }
    }

    constexpr Point operator-(Dir d) const {
        return operator+(-d);
    }

    constexpr bool operator==(const Point &other) const {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const Point &other) const {
        return !(*this == other);
    }
};

std::ostream &operator<<(std::ostream &ost, const Point &p) {
    ost << '(' << p.x << ',' << p.y << ')';
    return ost;
}

struct Node {
    Point p{}, prev{ -1,-1 };
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

struct GraphMock {
    Array2D<char> nodes{15, 10};

    GraphMock() {
        srand(5432);
        for (uint64_t i = 0; i < nodes.data.size(); ++i)
            nodes.data[i] = '0' + rand() % 10;
    }

    int64_t w() const { return nodes.w; }
    int64_t h() const { return nodes.h; }

    double distance(const Point &p, Dir d) const {
        return double(std::abs(nodes[p] - nodes[p + d]));
    }

    void print() const {
        nodes.print();
    }
};
