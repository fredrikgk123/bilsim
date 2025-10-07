# Create a library for core game logic components
add_library(core
    gameObject.cpp
    gameObject.hpp
    vehicle.cpp
    vehicle.hpp
    powerup.cpp
    powerup.hpp
    powerupManager.cpp
    powerupManager.hpp
)

target_include_directories(core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
