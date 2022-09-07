#include "image_graph.hpp"

DistanceGraph::DistanceGraph(const cv::Mat &imageLuminance,
                             PointFunc distanceFunc) :
    directional(false) {
    values[0] = distanceFunc(imageLuminance, 0);
}

DistanceGraph::DistanceGraph(const cv::Mat &imageLuminance,
                             DirFunc distanceFunc) :
    directional(true) {
    values = distanceFunc(imageLuminance);
}

double DistanceGraph::distance(const Point &p, Dir d) const {
    return values[d * directional].at<double>(p.y, p.x);
}
