cmake_minimum_required(VERSION 3.24)

project(Livewire
  VERSION 0.1.0
  LANGUAGES CXX)

include(CheckIPOSupported)
check_ipo_supported()

include(FetchContent)

FetchContent_Declare(
    lemon
    URL http://lemon.cs.elte.hu/pub/sources/lemon-1.3.1.tar.gz)

FetchContent_MakeAvailable(lemon)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui MultimediaWidgets)
find_package(OpenCV REQUIRED core imgcodecs imgproc)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17) # Lemon library compatibility
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(configured_files)
add_subdirectory(src)
