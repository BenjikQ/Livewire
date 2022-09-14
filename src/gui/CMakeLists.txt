set(PROJECT_SOURCES
        main.cpp
        image_viewer.cpp
        image_viewer.hpp
        image_viewer.ui
        paint_area.cpp
        paint_area.hpp
        presave_dialog.hpp
        presave_dialog.cpp
        comparison_display.hpp
        comparison_display.cpp
        image_label.hpp
        image_label.cpp
        io.hpp
        processing.hpp
        processing.cpp)

qt_add_executable(livewire_gui
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES})

target_link_libraries(livewire_gui
    PRIVATE
    Qt6::Widgets
    livewire)

set_target_properties(livewire_gui PROPERTIES
    WIN32_EXECUTABLE TRUE)

qt_finalize_executable(livewire_gui)
