# Create a library for UI components
add_library(ui STATIC
    imgui_layer.cpp
    imgui_context.cpp
)

target_include_directories(ui PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Include ImGui object files (created as an OBJECT library in the top-level CMake) so
# ui will contain ImGui implementation objects and resolve ImGui symbols at link time.
# This consumes the OBJECT library produced by the top-level `imgui` target.
target_sources(ui PRIVATE $<TARGET_OBJECTS:imgui>)

# Ensure OpenGL is available and link against required libraries
find_package(OpenGL REQUIRED)

# Link against required libraries (threepp provides GLFW/OpenGL via its targets)
target_link_libraries(ui PUBLIC
    threepp::threepp
    core
    OpenGL::GL
)

# If a glfw target exists, link it as well (threepp may provide it depending on configuration)
if(TARGET glfw)
    target_link_libraries(ui PUBLIC glfw)
    message(STATUS "ui: Linked to glfw target")
endif()

# Ensure consistent MSVC runtime library
if(MSVC)
    set_target_properties(ui PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
endif()