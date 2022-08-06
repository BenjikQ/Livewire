cmake_minimum_required(VERSION 3.13)

project(Livewire VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.1.0)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.6.2)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_WERROR OFF)
set(BENCHMARK_FORCE_WERROR OFF)
FetchContent_MakeAvailable(Catch2 googlebenchmark)

find_package(Boost REQUIRED)

add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(CTest)
    include(Catch)
endif()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

set(BUILD_BENCHMARKS ON)
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

set(BUILD_EXTRAS ON)
if(BUILD_EXTRAS)
    add_subdirectory(extras)
endif()
