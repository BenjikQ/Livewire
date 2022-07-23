#include "utils.hpp"

cv::Mat getTestImage(const char *fileName) {
    cv::Mat image = cv::imread(fileName), imageGray;
    cv::cvtColor(image, imageGray, cv::COLOR_BGR2GRAY);

    return imageGray;
}

void writeTestImage(const cv::Mat &img, const char *fileName) {
    cv::imwrite(fileName, img);
}

int main(int argc, char *argv[]) {
    const char *input = argc > 1 ? argv[1] : "input.png";
    const char *output = argc > 2 ? argv[2] : "output.png";

    cv::Mat m[4];
    m[0] = getTestImage(input);
    m[1] = gaussFilter(m[0]);
    m[2] = gradientSobel(m[1]);
    auto dc = dirCosts(m[0], m[2]);

    cv::imshow("a", dc[0]);
    cv::imshow("b", dc[1]);
    cv::imshow("c", dc[2]);
    cv::imshow("d", dc[3]);


    (void)cv::waitKey(0);
    return 0;
}
