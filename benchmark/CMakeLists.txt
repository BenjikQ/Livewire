add_executable(example_benchmark example_benchmark.cpp)
target_link_libraries(example_benchmark
    PRIVATE
        benchmark::benchmark_main
        benchmark::benchmark)
