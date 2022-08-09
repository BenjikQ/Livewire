#include "cost_functions.hpp"

#include "utils.hpp"

constexpr bool enableDebugData = true;
cv::Mat debugLastGrad, debugLastLzc;
DistanceGraph::DistArr debugLastDir;

DistanceGraph::DistArr basicCostFunc(const cv::Mat &img) {
    constexpr double gradWeight = 0.4;
    constexpr double lzcWeight = 0.4;
    constexpr double dirWeight = 0.2;

    cv::Mat smoothed, grad, lzc, grad_p_lzc;
    smoothed = gaussFilter(img);
    grad = gradientSobel(smoothed);
    laplacianZeroCrossing(smoothed).convertTo(lzc, CV_64F);
    grad_p_lzc = (1 - grad) * gradWeight + lzc * lzcWeight;
    auto dir = dirCosts(img, grad);

    if constexpr (enableDebugData) {
        debugLastGrad = 1 - grad;
        debugLastLzc = lzc;
        debugLastDir = dir;
    }

    for (int i = 0; i < dir.size(); ++i)
        dir[i] = dir[i] * dirWeight + grad_p_lzc;

    return dir;
}

std::array<cv::Mat, Dir::COUNT + 2> getDebugBlocks(cv::Point2i center,
                                                   int radius) {
    const cv::Point2i begin{ std::max(center.x - radius, 0),
                             std::max(center.y - radius, 0) };
    const cv::Point2i end{ std::min(center.x + radius, debugLastGrad.cols - 1),
                           std::min(center.y + radius,
                                    debugLastGrad.rows - 1) };
    const cv::Rect rect{ begin.x, begin.y, end.x - begin.x, end.y - begin.y };

    std::array<cv::Mat, 10> blocks{};

    blocks[0] = debugLastGrad(rect);
    blocks[1] = debugLastLzc(rect);
    for (uint i = 0; i < debugLastDir.size(); ++i)
        blocks[i + 2] = debugLastDir[i](rect);

    return blocks;
}
