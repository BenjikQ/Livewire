cmake_minimum_required(VERSION 3.13)

project(Livewire
  VERSION 0.1.0
  LANGUAGES CXX)

include(CheckIPOSupported)
check_ipo_supported()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(configured_files)
add_subdirectory(src)
