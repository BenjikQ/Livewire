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

    DistanceGraph(const cv::Mat &imageLuminance, Func distanceFunc) {
        const auto [vert_, horiz_, pdiag_, ndiag_] =
            distanceFunc(imageLuminance);
        vert = vert_;
        horiz = horiz_;
        pdiag = pdiag_;
        ndiag = ndiag_;
    }

    int64_t w() const { return vert.cols; }
    int64_t h() const { return horiz.rows; }

    double distance(const Point &p, Dir d) const {
        switch (d) {
        case Dir::RIGHT: return horiz.at<double>(p.y, p.x);
        case Dir::LEFT: return horiz.at<double>(p.y, p.x - 1);
        case Dir::TOPRIGHT: return pdiag.at<double>(p.y - 1, p.x);
        case Dir::BOTTOMLEFT: return pdiag.at<double>(p.y, p.x - 1);
        case Dir::TOP: return vert.at<double>(p.y - 1, p.x);
        case Dir::BOTTOM: return vert.at<double>(p.y, p.x);
        case Dir::TOPLEFT: return ndiag.at<double>(p.y - 1, p.x - 1);
        case Dir::BOTTOMRIGHT: return ndiag.at<double>(p.y, p.x);
        default: return INF;
        }
    }
};

#endif  // IMAGE_GRAPH_HPP
