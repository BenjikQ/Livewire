#include "utils.hpp"

cv::Mat gaussFilter(const cv::Mat &mat) {
    cv::Mat blurred;
    cv::GaussianBlur(mat, blurred, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
    return blurred;
}

cv::Mat laplacianZeroCrossing(const cv::Mat &blurred) {
    cv::Mat laplace, minLap, maxLap, zeroCrossing;

    // cv::GaussianBlur(mat, blurred, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
    cv::Laplacian(blurred, laplace, 3);

    // https://stackoverflow.com/a/48440931
    cv::morphologyEx(laplace, minLap, cv::MORPH_ERODE,
                     cv::Mat::ones(3, 3, CV_64F));
    cv::morphologyEx(laplace, maxLap, cv::MORPH_DILATE,
                     cv::Mat::ones(3, 3, CV_64F));

    zeroCrossing = (minLap<0 & laplace> 0) | (maxLap > 0 & laplace < 0);
    // cv::normalize(laplace, zeroCrossing, 0, 1, cv::NORM_MINMAX);
    return zeroCrossing * (1. / 255);
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

std::array<cv::Mat, 8> dirCosts(const cv::Mat &imgui8, const cv::Mat &grad) {
    constexpr auto acos_op = [](auto x) { return std::acos(x); };
    const auto w = imgui8.cols, h = imgui8.rows;
    const cv::Rect rect[4][2]{
        { { 0, 0, w - 1, h }, { 1, 0, w - 1, h } },
        { { 0, 1, w - 1, h - 1 }, { 1, 0, w - 1, h - 1 } },
        { { 0, 1, w, h - 1 }, { 0, 0, w, h - 1 } },
        { { 1, 1, w - 1, h - 1 }, { 0, 0, w - 1, h - 1 } }
    };

    std::array<cv::Mat, 8> vals;
    cv::Mat temp[3], img;

    imgui8.convertTo(img, CV_64F, 1. / 255);

    for (uint i = 0; i < vals.size(); ++i) {
        const uint ind1 = i & 3, ind2 = ((i & 4) != 0);
        temp[0] = cv::abs(img(rect[ind1][ind2]) - img(rect[ind1][!ind2]));
        temp[1] = grad(rect[ind1][ind2]).mul(temp[0]);
        temp[2] = grad(rect[ind1][!ind2]).mul(temp[0]);
        std::transform(temp[1].begin<double>(), temp[1].end<double>(),
                       temp[1].begin<double>(), acos_op);
        std::transform(temp[2].begin<double>(), temp[2].end<double>(),
                       temp[2].begin<double>(), acos_op);
        temp[0] = (temp[1] + temp[2]) * std::numbers::inv_pi;

        cv::copyMakeBorder(temp[0], vals[i], rect[ind1][ind2].y,
                           rect[ind1][!ind2].y, rect[ind1][ind2].x,
                           rect[ind1][!ind2].x, cv::BORDER_CONSTANT, 1.0);
    }

    return vals;
}

cv::Mat nonlinearGradient(const cv::Mat &grad) {
    const double maxG =
        *std::max_element(grad.begin<double>(), grad.end<double>());
    const auto normG = grad / maxG;

    cv::Mat expMat = normG.mul(normG) * -4.0;
    cv::exp(expMat, expMat);

    return expMat;
}
