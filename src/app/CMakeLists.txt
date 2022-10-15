include_directories(${CMAKE_BINARY_DIR}/configured_files)

qt_add_executable(main
  main.cpp
  main_window.ui
  main_window.cpp main_window.hpp
  commands.cpp commands.hpp
  point_item.cpp point_item.hpp
  painter_options.cpp painter_options.hpp
  path_item.cpp path_item.hpp
  imgprc_helpers.hpp imgprc_helpers.cpp
  selection_layer_item.hpp selection_layer_item.cpp
  presave_dialog.hpp presave_dialog.cpp presave_dialog.ui
  image_label.hpp image_label.cpp
  comparison_display.hpp comparison_display.cpp comparison_display.ui)

target_link_libraries(main
  PUBLIC
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
  PRIVATE
    livewire
    Qt6::Widgets
    Qt6::Gui
    Qt6::MultimediaWidgets
    opencv_videoio)

set_target_properties(main
  PROPERTIES
    OUTPUT_NAME livewire
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    INTERPROCEDURAL_OPTIMIZATION TRUE)

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
    "../../data/icons/redo.png"
    "../../data/icons/brush.png"
    "../../data/icons/point.png"
    "../../data/icons/pause.png"
    "../../data/icons/play.png"
    "../../data/icons/next.png"
    "../../data/icons/prev.png"
    "../../data/icons/plus.png"
    "../../data/icons/minus.png")

# Shouldn't probably be needed
# but didn't work without copying files
add_custom_target(copy_icons
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)
add_dependencies(main copy_icons)
