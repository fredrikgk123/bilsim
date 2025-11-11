# Create a library for graphics components
add_library(graphics
        game_object_renderer.cpp
        game_object_renderer.hpp
        vehicle_renderer.cpp
        vehicle_renderer.hpp
        powerup_renderer.cpp
        powerup_renderer.hpp
        obstacle_renderer.cpp
        obstacle_renderer.hpp
        scene_manager.cpp
        scene_manager.hpp
)

# Make headers available to other directories
target_include_directories(graphics PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src  # Add parent src directory for cross-module includes
        ${CMAKE_SOURCE_DIR}/src/core/interfaces  # Add interfaces directory
)

# Link against the core library for Vehicle class access
target_link_libraries(graphics PUBLIC
        core
        threepp::threepp
)
