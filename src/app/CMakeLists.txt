add_executable(main main.cpp)
set_target_properties(main PROPERTIES OUTPUT_NAME livewire)
target_link_libraries(main PRIVATE livewire)
