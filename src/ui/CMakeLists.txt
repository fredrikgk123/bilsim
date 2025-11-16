# Create a library for UI components
add_library(ui
    imgui_layer.cpp
    imgui_context.cpp
)

# Make headers available to other directories
target_include_directories(ui PUBLIC
    ${CMAKE_SOURCE_DIR}/include
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

# Ensure imgui is built before ui
add_dependencies(ui imgui)

