# Create a library for graphics components
add_library(graphics
        gameObjectRenderer.cpp
        gameObjectRenderer.hpp
        vehicleRenderer.cpp
        vehicleRenderer.hpp
        powerupRenderer.cpp
        powerupRenderer.hpp
        sceneManager.cpp
        sceneManager.hpp
)

# Make headers available to other directories
target_include_directories(graphics PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against the core library for Vehicle class access
target_link_libraries(graphics PUBLIC
        core
        threepp::threepp
)
