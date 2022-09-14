add_library(livewire
    graph.cpp graph.hpp
  cost_functions.cpp cost_functions.hpp)

target_include_directories(livewire
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${lemon_BINARY_DIR}
    ${lemon_SOURCE_DIR})

target_link_libraries(livewire
  INTERFACE
    lemon
  PUBLIC
    opencv_core
    opencv_imgcodecs
    opencv_imgproc)

set_target_properties(livewire PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
