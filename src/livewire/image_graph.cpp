#include "image_graph.hpp"

DistanceGraph::DistanceGraph(const cv::Mat &imageLuminance,
                             Func &&distanceFunc) {
    const auto [vert_, horiz_, pdiag_, ndiag_] = distanceFunc(imageLuminance);
    vert = vert_;
    horiz = horiz_;
    pdiag = pdiag_;
    ndiag = ndiag_;
}

double DistanceGraph::distance(const Point &p, Dir d) const {
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
