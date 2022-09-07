#include "dijkstra.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <sstream>

#include "typedefs.hpp"

class TestGrid {
    int width, height;
    Array2D<int> nodes;

public:
    TestGrid(int width, int height, const std::vector<Point> obstacles) :
        width{ width }, height{ height }, nodes{ width, height, 1 } {
        for (const auto& obstacle : obstacles) {
            nodes[obstacle] = 10000000;
        }
    }

    int w() const { return width; }
    int h() const { return height; }

    std::vector<Point> neighbors(const Point& p) const {
        std::vector<Point> result;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i != 0 || j != 0) {
                    Point q{ p.x + i, p.y + j };
                    if (q.x >= 0 && q.y >= 0 && q.x < width && q.y < height) {
                        result.push_back(q);
                    }
                }
            }
        }
        return result;
    }

    // Assume they are neighbors
    int distance(const Point& p, Dir dir) const { return nodes[p + dir]; }
};

std::string print(const std::vector<Point>& path) {
    std::stringstream ss;
    for (const auto& point : path) {
        ss << point << ' ';
    }
    return ss.str();
}

TEST_CASE("Calculating shortest path using dijkstra algorithm") {
    const auto [graph, source, dest,
                expected] = GENERATE(table<TestGrid, Point, Point, int>({
        { TestGrid{ 1, 1, {} }, { 0, 0 }, { 0, 0 }, 1 },
        { TestGrid{ 2, 2, {} }, { 0, 0 }, { 1, 1 }, 2 },
        { TestGrid{ 3, 4, {} }, { 0, 0 }, { 2, 2 }, 3 },
        { TestGrid{ 4, 5, { { 1, 2 }, { 2, 2 }, { 1, 3 } } },
          { 0, 0 },
          { 2, 4 },
          6 },
        { TestGrid{
              11,
              10,
              { { 4, 4 }, { 5, 4 }, { 5, 5 }, { 5, 6 }, { 6, 6 }, { 6, 7 } } },
          { 2, 2 },
          { 8, 8 },
          9 },
        { TestGrid{ 13,
                    13,
                    {
                        { 7, 4 },
                        { 8, 4 },
                        { 9, 4 },
                        { 10, 4 },
                        { 11, 4 },
                        { 5, 5 },
                        { 6, 5 },
                        { 7, 5 },
                        { 11, 5 },
                        { 5, 6 },
                        { 11, 6 },
                        { 5, 7 },
                        { 5, 8 },
                        { 6, 8 },
                        { 6, 9 },
                        { 7, 9 },
                        { 8, 9 },
                        { 8, 10 },
                    } },
          { 3, 2 },
          { 9, 8 },
          14 },
        { TestGrid{ 15,
                    10,
                    {
                        { 3, 1 },  { 4, 1 },  { 5, 1 },  { 5, 2 },  { 4, 3 },
                        { 5, 3 },  { 4, 4 },  { 3, 4 },  { 2, 4 },  { 1, 5 },
                        { 2, 5 },  { 12, 2 }, { 13, 2 }, { 10, 3 }, { 11, 3 },
                        { 12, 3 }, { 10, 4 }, { 10, 5 }, { 10, 6 }, { 9, 6 },
                    } },
          { 1, 2 },
          { 12, 5 },
          15 },
    }));

    const auto path = dijkstra(graph, source, dest);
    const auto distance = path.size();

    INFO("Path: " << print(path));
    CHECK(distance == expected);
}
