set(PROJECT_SOURCES
        main.cpp
        image_viewer.cpp
        image_viewer.h
        image_viewer.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(livewire_gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(livewire_gui
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(livewire_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(livewire_gui PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(livewire_gui)
endif()
