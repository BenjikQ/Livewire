#ifndef COST_FUNCTIONS_HPP
#define COST_FUNCTIONS_HPP

#include "image_graph.hpp"

DistanceGraph::DistArr basicCostFunc(const cv::Mat &img);
std::array<cv::Mat, Dir::COUNT + 2> getDebugBlocks(cv::Point2i center,
                                                   int radius);

#endif  // COST_FUNCTIONS_HPP
