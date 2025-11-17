cmake_minimum_required(VERSION 3.21)
project(bilsim)

set(CMAKE_CXX_STANDARD 20)

# Set Ninja as the preferred generator if available
if(NOT CMAKE_MAKE_PROGRAM)
    find_program(NINJA_EXECUTABLE ninja PATHS /opt/homebrew/Cellar/ninja/*/bin /opt/homebrew/bin /usr/local/bin)
    if(NINJA_EXECUTABLE)
        set(CMAKE_MAKE_PROGRAM ${NINJA_EXECUTABLE} CACHE FILEPATH "Ninja build tool" FORCE)
    endif()
endif()

# Add optimization flags for better performance
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

set(THREEPP_BUILD_TESTS OFF)
set(THREEPP_BUILD_EXAMPLES OFF)

FetchContent_Declare(
    threepp
    GIT_REPOSITORY https://github.com/markaren/threepp.git
    GIT_TAG 619cdaacc2e7dc6ea709d8c7f482e2994af7349d
)

# Fetch miniaudio as a single header file
FetchContent_Declare(
    miniaudio
    URL https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
    DOWNLOAD_NO_EXTRACT TRUE
)

# Fetch Catch2 for unit testing
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.4
)

FetchContent_MakeAvailable(threepp Catch2)
FetchContent_MakeAvailable(imgui)

# Create imgui library target that ui can link to
# This ensures GLFW and OpenGL are properly linked
if(DEFINED imgui_SOURCE_DIR)
    message(STATUS "Creating ImGui library from: ${imgui_SOURCE_DIR}")

    find_package(OpenGL REQUIRED)

    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )

    target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )

    # Link imgui to threepp to get GLFW and OpenGL
    target_link_libraries(imgui PUBLIC threepp::threepp OpenGL::GL)

    # Try to link glfw if available as a target
    if(TARGET glfw)
        target_link_libraries(imgui PUBLIC glfw)
        message(STATUS "ImGui: Linked to glfw target")
    endif()

    target_compile_features(imgui PUBLIC cxx_std_17)
    set_target_properties(imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)

    if(MSVC)
        set_target_properties(imgui PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
        message(STATUS "ImGui: Configured for MSVC with matching runtime library")
    endif()

    message(STATUS "ImGui library target created successfully")
else()
    message(FATAL_ERROR "imgui_SOURCE_DIR not defined - FetchContent failed to download ImGui")
endif()

FetchContent_GetProperties(miniaudio)
if(NOT miniaudio_POPULATED)
    FetchContent_Populate(miniaudio)
endif()

add_subdirectory(src)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Copy assets directory to build directory
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/src/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/tests/assets
    COMMENT "Copying assets to build directory"
    VERBATIM
)
add_dependencies(bilsim copy_assets)

message(STATUS "bilsim configured successfully with audio support (miniaudio)")
message(STATUS "Catch2 testing framework configured")
