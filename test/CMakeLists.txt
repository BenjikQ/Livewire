add_executable(tests test_main.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
add_test(my_tests tests)
