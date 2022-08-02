#ifndef UTILS_HPP
#define UTILS_HPP

#include <numbers>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat gaussFilter(const cv::Mat &mat);
cv::Mat laplacianZeroCrossing(const cv::Mat &blurred);
cv::Mat gradientSobel(const cv::Mat &blurred);
cv::Mat edgesCanny(const cv::Mat &img);

std::array<cv::Mat, 4> dirCosts(const cv::Mat &imgui8, const cv::Mat &grad);

#endif  // UTILS_HPP
