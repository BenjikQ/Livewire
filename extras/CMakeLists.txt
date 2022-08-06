add_executable(image_processing image_processing.cpp)
target_link_libraries(image_processing PRIVATE livewire)

add_executable(fibonacci_heap fibonacci_heap.cpp)
target_link_libraries(fibonacci_heap PRIVATE Boost::boost)
