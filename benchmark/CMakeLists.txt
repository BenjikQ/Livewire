add_executable(dijkstra_benchmark dijkstra_benchmark.cpp)
target_link_libraries(dijkstra_benchmark
    PRIVATE
        benchmark::benchmark_main
        benchmark::benchmark
        livewire)

configure_file(${PROJECT_SOURCE_DIR}/data/dijkstra.jpg
    ${CMAKE_CURRENT_BINARY_DIR}/dijkstra.jpg COPYONLY)
