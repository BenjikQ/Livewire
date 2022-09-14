#ifndef LIVEWIRE_COST_FUNCTIONS_HPP
#define LIVEWIRE_COST_FUNCTIONS_HPP

// clang-format off
namespace cv { class Mat; }
// clang-format on

struct CostFunction {
    cv::Mat operator()(const cv::Mat &image);
};

#endif  // LIVEWIRE_COST_FUNCTIONS_HPP
