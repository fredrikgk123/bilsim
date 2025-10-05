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

# Fetch miniaudio as a single header file (more reliable)
FetchContent_Declare(
    miniaudio
    URL https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
    DOWNLOAD_NO_EXTRACT TRUE
)

FetchContent_MakeAvailable(threepp)
FetchContent_GetProperties(miniaudio)
if(NOT miniaudio_POPULATED)
    FetchContent_Populate(miniaudio)
endif()

add_subdirectory(src)
target_link_libraries(bilsim PUBLIC threepp::threepp)

message(STATUS "bilsim configured successfully with audio support (miniaudio)")
