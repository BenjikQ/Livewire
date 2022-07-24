find_package(OpenCV REQUIRED)

add_library(livewire INTERFACE)

target_link_libraries(livewire INTERFACE ${OpenCV_LIBS})
target_include_directories(livewire INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_sources(livewire
    INTERFACE
        dijkstra.hpp
        image_graph.hpp
        typedefs.hpp
        utils.hpp)
