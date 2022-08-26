#ifndef IMAGE_GRAPH_HPP
#define IMAGE_GRAPH_HPP

#include <functional>
#include <opencv2/core.hpp>

#include "typedefs.hpp"

class DistanceGraph {
public:
    using DistArr = std::array<cv::Mat, Dir::COUNT>;
    using PointArr = cv::Mat;
    using DirFunc = std::function<DistArr(const cv::Mat &)>;
    using PointFunc = std::function<PointArr(const cv::Mat &, int)>;

private:
    DistArr values;
    bool directional;

public:
    DistanceGraph() = default;
    DistanceGraph(const cv::Mat &imageLuminance, PointFunc distanceFunc);
    DistanceGraph(const cv::Mat &imageLuminance, DirFunc distanceFunc);

    int64_t w() const { return values[0].cols; }
    int64_t h() const { return values[0].rows; }

    double distance(const Point &p, Dir d = {}) const;
};

#endif  // IMAGE_GRAPH_HPP
