find_package(OpenCV REQUIRED)

add_library(livewire)

target_link_libraries(livewire
    PUBLIC
        opencv_core
        opencv_imgcodecs
        opencv_imgproc)
target_include_directories(livewire PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(livewire
    PRIVATE
        cost_functions.cpp
        dijkstra.cpp
        image_graph.cpp
        typedefs.cpp
        utils.cpp
        regions.cpp
    PUBLIC
        cost_functions.hpp
        dijkstra.hpp
        image_graph.hpp
        typedefs.hpp
        utils.hpp
        regions.hpp)
