#include "cost_functions.hpp"

#include "utils.hpp"

DistanceGraph::DistQuad basicCostFunc(const cv::Mat &img) {
    constexpr double gradWeight = 0.4;
    constexpr double lzcWeight = 0.4;
    constexpr double dirWeight = 0.2;

    cv::Mat smoothed, grad, lzc, grad_p_lzc;
    smoothed = gaussFilter(img);
    grad = gradientSobel(smoothed);
    laplacianZeroCrossing(smoothed).convertTo(lzc, CV_64F);
    grad_p_lzc = (1 - grad) * gradWeight + lzc * lzcWeight;
    auto dir = dirCosts(img, grad);

    for (int i = 0; i < 4; ++i)
        dir[i] = dir[i] * dirWeight;
    for (int i = 0; i < 4; ++i)
        dir[i] = dir[i] + grad_p_lzc({ 0, 0, dir[i].cols, dir[i].rows });

    return { dir[0], dir[1], dir[2], dir[3] };
}
