# Create a library for UI components
add_library(ui
    imgui_layer.cpp
    imgui_layer.hpp
    imgui_context.cpp
    imgui_context.hpp
)

# Make headers available to other directories
target_include_directories(ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/core  # Add core module
    ${CMAKE_SOURCE_DIR}/src/core/interfaces  # Add interfaces for IVehicleState
)

# If FetchContent provided imgui, add its source dir to includes so <imgui.h> resolves
if(DEFINED imgui_SOURCE_DIR)
    target_include_directories(ui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
endif()

# Link against required libraries
target_link_libraries(ui PUBLIC
    threepp::threepp
    core  # Link against core library for Vehicle class
    imgui  # Link against imgui library for ImGui functions and backend implementations
)
