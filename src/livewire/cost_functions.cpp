#include "cost_functions.hpp"

#include "utils.hpp"

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

    for (int i = 0; i < dir.size(); ++i)
        dir[i] = dir[i] * dirWeight + grad_p_lzc;

    return dir;
}
