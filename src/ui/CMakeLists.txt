# Create a library for UI components
add_library(ui
    imgui_layer.cpp
    imgui_layer.hpp
)

# Make headers available to other directories
target_include_directories(ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/core  # Add core module for vehicle.hpp
)

# If FetchContent provided imgui, add its source dir to includes so <imgui.h> resolves
if(DEFINED imgui_SOURCE_DIR)
    target_include_directories(ui PUBLIC ${imgui_SOURCE_DIR})
endif()

# Link against required libraries
target_link_libraries(ui PUBLIC
    threepp::threepp
    core  # Link against core library for Vehicle class
    # Note: the final executable links imgui; avoid requiring imgui target here to prevent
    # CMake ordering issues during generation.
)
