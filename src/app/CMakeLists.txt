include_directories(${CMAKE_BINARY_DIR}/configured_files)

qt_add_executable(main
  MANUAL_FINALIZATION
  main.cpp
  main_window.ui
  main_window.cpp
  main_window.hpp)
target_link_libraries(main
  PRIVATE
    livewire
    Qt6::Core
    Qt6::Widgets)
set_target_properties(main
  PROPERTIES
    OUTPUT_NAME livewire
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON)
qt_finalize_executable(main)
