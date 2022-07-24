add_subdirectory(livewire)
add_subdirectory(app)

set(BUILD_EXAMPLE ON)
if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
