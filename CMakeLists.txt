cmake_minimum_required(VERSION 3.21)
project(bilsim)

set(CMAKE_CXX_STANDARD 20)

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

FetchContent_MakeAvailable(threepp Catch2)
FetchContent_GetProperties(miniaudio)
if(NOT miniaudio_POPULATED)
    FetchContent_Populate(miniaudio)
endif()

add_subdirectory(src)
target_link_libraries(bilsim PUBLIC threepp::threepp)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Copy assets directory to build directory
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/src/assets
    COMMENT "Copying assets to build directory"
    VERBATIM
)
add_dependencies(bilsim copy_assets)

message(STATUS "bilsim configured successfully with audio support (miniaudio)")
message(STATUS "Catch2 testing framework configured")
