include_directories(${CMAKE_BINARY_DIR}/configured_files)

qt_add_executable(main
  MANUAL_FINALIZATION
  main.cpp
  main_window.ui
  main_window.cpp
  main_window.hpp
  commands.cpp
  commands.h)
target_link_libraries(main
  PRIVATE
    livewire
    Qt6::Widgets
    Qt6::Gui)
set_target_properties(main
  PROPERTIES
    OUTPUT_NAME livewire
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    INTERPROCEDURAL_OPTIMIZATION TRUE)
qt_finalize_executable(main)

qt_add_resources(main "icons"
  PREFIX
    "/icons"
  FILES
    "../../data/icons/mouse-coordinates.png"
    "../../data/icons/new.png"
    "../../data/icons/save.png"
    "../../data/icons/screen.png"
    "../../data/icons/app.png"
    "../../data/icons/undo.png"
    "../../data/icons/redo.png")
