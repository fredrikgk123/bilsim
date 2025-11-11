# Create a library for core game logic components
add_library(core
    game_object.cpp
    game_object.hpp
    vehicle.cpp
    vehicle.hpp
    powerup.cpp
    powerup.hpp
    powerup_manager.cpp
    powerup_manager.hpp
    obstacle.cpp
    obstacle.hpp
    obstacle_manager.cpp
    obstacle_manager.hpp
    game.cpp
    game.hpp
)

target_include_directories(core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
)

target_link_libraries(core PUBLIC threepp::threepp)

