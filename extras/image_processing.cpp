#include "cost_functions.hpp"
#include "dijkstra.hpp"
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
    const char *input = argc > 1 ? argv[1] : "test_image2.png";
    //    const char *output = argc > 2 ? argv[2] : "output.png";

    cv::Mat testImg = getTestImage(input);
    DistanceGraph graph{ testImg, basicCostFunc };
    auto path = dijkstra(graph, { 20, 0 }, { 0, 20 });
    //     for (const auto &x : path)
    //         std::cout << x << "  ";
    //     const auto [c0, c1, c2, c3] = basicCostFunc(testImg);
    //     cv::Mat m[4];
    //     m[0] = getTestImage(input);
    //     m[1] = gaussFilter(m[0]);
    //     m[2] = gradientSobel(m[1]);
    //     auto dc = dirCosts(m[0], m[2]);

    //    cv::imshow("a", c0);
    //    cv::imshow("b", c1);
    //    cv::imshow("c", c2);
    //    cv::imshow("d", c3);


    //    (void)cv::waitKey(0);
    return 0;
}
