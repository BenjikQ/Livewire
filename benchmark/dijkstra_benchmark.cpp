#include <benchmark/benchmark.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "cost_functions.hpp"
#include "dijkstra.hpp"

cv::Mat getTestImage(const char* fileName) {
    cv::Mat image = cv::imread(fileName), imageGray;
    cv::cvtColor(image, imageGray, cv::COLOR_BGR2GRAY);

    return imageGray;
}

static void BM_SomeFunction(benchmark::State& state) {
    const cv::Mat image = getTestImage("dijkstra.jpg");
    const DistanceGraph graph{ image, basicCostFunc };

    for (auto _ : state) {
        const auto path = dijkstra(graph, { 20, 0 }, { 0, 20 });
        benchmark::DoNotOptimize(path);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction);
// Run the benchmark
BENCHMARK_MAIN();
