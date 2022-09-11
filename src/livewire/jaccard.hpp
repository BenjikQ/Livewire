#ifndef JACCARD_HPP
#define JACCARD_HPP

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat readBinaryImage(const std::string &fileName) {
    cv::Mat inImage = cv::imread(fileName), binImage;
    cv::threshold(inImage, binImage, 127, 255, cv::THRESH_BINARY);
    return binImage;
}

double calcJaccardCoeff(const cv::Mat &binImg1, const cv::Mat &binImg2) {
    constexpr auto predicate = [](uint8_t x) { return x != 0; };

    if (binImg1.size != binImg2.size) return -1;
    const auto boolImg1 = (binImg1 > 0), boolImg2 = (binImg2 > 0);
    const auto conj = boolImg1 & boolImg2, alt = boolImg1 | boolImg2;
    return 1.0 *
           std::count_if(conj.cbegin<uint8_t>(), conj.cend<uint8_t>(),
                         predicate) /
           std::count_if(alt.cbegin<uint8_t>(), alt.cend<uint8_t>(), predicate);
}

#endif  // JACCARD_HPP
