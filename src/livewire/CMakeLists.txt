find_package(OpenCV REQUIRED)

add_library(livewire)

target_link_libraries(livewire PUBLIC opencv_core)
target_include_directories(livewire PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_sources(livewire
    PRIVATE
        cost_functions.hpp
        dijkstra.hpp
        image_graph.hpp
        typedefs.hpp
        utils.hpp
    PUBLIC
        cost_functions.cpp
        dijkstra.cpp
        image_graph.cpp
        typedefs.cpp
        utils.cpp)
