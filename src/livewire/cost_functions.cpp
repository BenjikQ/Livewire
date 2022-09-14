#include "cost_functions.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat gaussFilter(const cv::Mat &mat) {
    cv::Mat blurred;
    cv::GaussianBlur(mat, blurred, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
    return blurred;
}

cv::Mat gradientSobel(const cv::Mat &blurred) {
    cv::Mat grad_x, grad_y, grad_x2, grad_y2, grad, gradnorm;

    cv::Sobel(blurred, grad_x, CV_64F, 1, 0, 1, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(blurred, grad_y, CV_64F, 0, 1, 1, 1, 0, cv::BORDER_DEFAULT);
    cv::pow(grad_x, 2, grad_x2);
    cv::pow(grad_y, 2, grad_y2);
    cv::sqrt(grad_x2 + grad_y2, grad);
    cv::normalize(grad, gradnorm, 0, 1, cv::NORM_MINMAX);

    return gradnorm;
}

cv::Mat edgesCanny(const cv::Mat &img) {
    cv::Mat edges, result;
    cv::Canny(img, edges, 60, 110, 3, true);
    edges.convertTo(result, CV_64F, -1. / 255, 1);

    return result;
}

cv::Mat nonlinearGradient(const cv::Mat &grad) {
    const double maxG = *std::max_element(grad.begin<double>(), grad.end<double>());
    const auto normG = grad / maxG;

    cv::Mat expMat = normG.mul(normG) * -4.0;
    cv::exp(expMat, expMat);

    return expMat;
}

cv::Mat CostFunction::operator()(const cv::Mat &image) {
    constexpr double gradWeight = 0.2;
    constexpr double cannWeight = 0.3;
    constexpr double nlgWeight = 0.5;

    cv::Mat input = gaussFilter(image);
    cv::Mat grad = gradientSobel(input);
    cv::Mat canny = edgesCanny(image);
    cv::Mat nlg = nonlinearGradient(grad);

    return grad * gradWeight + canny * cannWeight + nlg * nlgWeight;
}
