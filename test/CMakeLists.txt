add_executable(example_test example_test.cpp)
target_link_libraries(example_test PRIVATE Catch2::Catch2WithMain)
add_test(NAME example_test COMMAND example_test)

catch_discover_tests(example_test)
