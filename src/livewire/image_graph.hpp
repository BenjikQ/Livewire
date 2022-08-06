#ifndef IMAGE_GRAPH_HPP
#define IMAGE_GRAPH_HPP

#include <functional>
#include <opencv2/core.hpp>

#include "typedefs.hpp"

class DistanceGraph {
public:
    using DistQuad = std::tuple<cv::Mat, cv::Mat, cv::Mat, cv::Mat>;
    using Func = std::function<DistQuad(const cv::Mat &)>;

private:
    cv::Mat vert, horiz, pdiag, ndiag;

public:
    DistanceGraph() = default;

    DistanceGraph(const cv::Mat &imageLuminance, Func &&distanceFunc);

    int64_t w() const { return vert.cols; }
    int64_t h() const { return horiz.rows; }

    double distance(const Point &p, Dir d) const;
};

#endif  // IMAGE_GRAPH_HPP
