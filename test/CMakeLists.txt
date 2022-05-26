add_executable(tests testmain.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
add_test(my_tests tests)
