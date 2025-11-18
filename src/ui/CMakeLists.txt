# Find threepp's ImGui source directory
# Try multiple approaches to find the correct path
set(IMGUI_DIR "")

# Approach 1: Check in FetchContent's threepp source directory
if(EXISTS "${CMAKE_BINARY_DIR}/_deps/threepp-src/examples/external/imgui/imgui.cpp")
    set(IMGUI_DIR "${CMAKE_BINARY_DIR}/_deps/threepp-src/examples/external/imgui")
endif()

# Approach 2: If threepp is a local directory, check there
if(NOT IMGUI_DIR AND EXISTS "${CMAKE_SOURCE_DIR}/../threepp/examples/external/imgui/imgui.cpp")
    set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/../threepp/examples/external/imgui")
endif()

# Verify ImGui directory exists
if(NOT IMGUI_DIR OR NOT EXISTS "${IMGUI_DIR}/imgui.cpp")
    message(FATAL_ERROR "ImGui sources not found. Expected at: ${CMAKE_BINARY_DIR}/_deps/threepp-src/examples/external/imgui/")
endif()

message(STATUS "Using ImGui from: ${IMGUI_DIR}")

# ImGui source files
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
)

# Create a library for UI components with ImGui compiled in
add_library(ui STATIC
    imgui_layer.cpp
    imgui_context.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(ui PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${IMGUI_DIR}
)

# Link required libraries
target_link_libraries(ui PUBLIC
    threepp::threepp
    core
)

# Link to GLFW for ImGui backends (threepp provides this)
if(TARGET glfw)
    target_link_libraries(ui PRIVATE glfw)
elseif(TARGET glfw3)
    target_link_libraries(ui PRIVATE glfw3)
endif()

# Ensure consistent MSVC runtime library
if(MSVC)
    set_target_properties(ui PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
endif()