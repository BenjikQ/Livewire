set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    image_viewer.cpp
    image_viewer.hpp
    image_viewer.ui
    main.cpp
    paint_area.cpp
    paint_area.hpp)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(app
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES})
else()
    add_executable(app ${PROJECT_SOURCES})
endif()

target_link_libraries(app
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        livewire)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(app)
endif()
