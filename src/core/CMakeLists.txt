# Create a library for core game logic components
add_library(core
    game_object.cpp
    vehicle.cpp
    powerup.cpp
    powerup_manager.cpp
    obstacle.cpp
    obstacle_manager.cpp
    game.cpp
)

target_include_directories(core PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(core PUBLIC threepp::threepp)

