set(PROJECT_SOURCES
    main.cpp
    imageviewer.cpp
    imageviewer.h
    imageviewer.ui
    paintarea.cpp
    paintarea.h
    dijkstra.hpp
    image_graph.hpp
    lw_typedefs.hpp
    lw_utils.hpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Livewire
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Livewire APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Livewire SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Livewire
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Livewire PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Livewire PRIVATE ${OpenCV_LIBS})
target_include_directories(Livewire PRIVATE ${OpenCV_DIR}/include/opencv2)

set_target_properties(Livewire PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Livewire)
endif()
