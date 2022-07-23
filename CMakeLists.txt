cmake_minimum_required(VERSION 3.13)

project(Livewire
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

Include(FetchContent)

add_subdirectory(src/livewire)
add_subdirectory(src/app)

string(
        COMPARE EQUAL
        "${CMAKE_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}"
        PROJECT_IS_TOP_LEVEL)

option(ENABLE_TESTING "Enable the tests" ${PROJECT_IS_TOP_LEVEL})
if(ENABLE_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()
