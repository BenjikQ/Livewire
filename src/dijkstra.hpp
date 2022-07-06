#pragma once
#include <cstdint>
#include <vector>
#include <queue>
#include <iostream>
#pragma warning (disable: 26812)

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
std::vector<Point> dijkstra(const T &graph, const Point &source, const Point &dest) {
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
			if (p.x < 0 || p.y < 0 || p.x >= graph.w() || p.y >= graph.h() || visited[p])
				continue;
			double newDist = INF;
			if ((newDist = dist[current] + graph.distance(current, d)) < dist[p]) {
				dist[p] = newDist;
				prev[p] = current;
			}
		}
		current = nextOrigin(dist, visited);
	}

	return compilePath(prev, dest);
}
