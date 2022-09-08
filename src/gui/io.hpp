#ifndef IO_HPP
#define IO_HPP

#include <fstream>
#include <type_traits>

template <typename T>
bool writePointsToTextC(const T &iterable, const char *fileName) {
    std::ofstream outFile(fileName, std::ios::out);

    for (const auto &point : iterable) {
        outFile << point.x() << ' ' << point.y() << '\n';
    }

    return true;
}

template <typename T>
bool writePointsToText(const T &iterable, const char *fileName) {
    std::ofstream outFile(fileName, std::ios::out);

    for (const auto &point : iterable) {
        outFile << point.x << ' ' << point.y << '\n';
    }

    return true;
}

template <typename T>
T readPointsInto(const char *fileName) {
    std::ifstream inFile(fileName, std::ios::in);
    T ret{};
    int x, y;

    while (inFile >> x >> y)
        ret.push_back({ x, y });

    return ret;
}


#endif  // IO_HPP
