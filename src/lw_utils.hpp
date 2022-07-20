#pragma once
#include "lw_typedefs.hpp"
#include <core.hpp>
#include <imgproc.hpp>
#include <imgcodecs.hpp>
#include <highgui.hpp>

cv::Mat getTestImage() {
    constexpr const char *fileName = "test.png";

    cv::Mat image = cv::imread(fileName), imageGray;
    cv::cvtColor(image, imageGray, cv::COLOR_BGR2GRAY);

    return imageGray;
}

void writeTestImage(const cv::Mat &img) {
    constexpr const char *fileName = "test_out.png";

    cv::imwrite(fileName, img);
}

cv::Mat gaussFilter(const cv::Mat &mat) {
    cv::Mat blurred;
    cv::GaussianBlur(mat, blurred, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
    return blurred;
}

cv::Mat laplacianZeroCrossing(const cv::Mat &blurred) {
    cv::Mat laplace, minLap, maxLap, zeroCrossing;

    //cv::GaussianBlur(mat, blurred, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
    cv::Laplacian(blurred, laplace, 3);

    // https://stackoverflow.com/a/48440931
    cv::morphologyEx(laplace, minLap, cv::MORPH_ERODE, cv::Mat::ones(3, 3, CV_64F));
    cv::morphologyEx(laplace, maxLap, cv::MORPH_DILATE, cv::Mat::ones(3, 3, CV_64F));

    zeroCrossing = (minLap < 0 & laplace > 0) | (maxLap > 0 & laplace < 0);

    return zeroCrossing;
}

cv::Mat gradientSobel(const cv::Mat &blurred) {
    cv::Mat grad_x, grad_y, grad_x2, grad_y2, grad, gradnorm;

    cv::Sobel(blurred, grad_x, CV_64F, 1, 0, 1, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(blurred, grad_y, CV_64F, 0, 1, 1, 1, 0, cv::BORDER_DEFAULT);
    cv::pow(grad_x, 2, grad_x2);
    cv::pow(grad_y, 2, grad_y2);
    cv::sqrt(grad_x2 + grad_y2, grad);
    cv::normalize(grad, gradnorm, 0, 1, cv::NORM_MINMAX);

    return 1 - gradnorm;
}

int debugMain() {
    cv::Mat m[4];
    m[0] = getTestImage();
    m[1] = gaussFilter(m[0]);
    m[2] = gradientSobel(m[1]);

    cv::imshow("a", m[2]);


    (void)cv::waitKey(0);
    return 0;
}

