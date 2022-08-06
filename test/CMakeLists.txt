add_executable(dijkstra_test dijkstra_test.cpp)
target_link_libraries(dijkstra_test PRIVATE Catch2::Catch2WithMain livewire)
add_test(NAME dijkstra_test COMMAND dijkstra_test)

catch_discover_tests(dijkstra_test)
