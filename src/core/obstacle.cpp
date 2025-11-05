#include "obstacle.hpp"

namespace {
    // Obstacle size constants
    constexpr float WALL_LENGTH = 5.0f;
    constexpr float WALL_HEIGHT = 5.0f;
    constexpr float WALL_THICKNESS = 2.0f;

    constexpr float TREE_COLLISION_RADIUS = 0.5f;
    constexpr float TREE_HEIGHT = 6.0f;
}

Obstacle::Obstacle(float x, float y, float z, ObstacleType type, WallOrientation orientation)
    : GameObject(x, y, z), type_(type), orientation_(orientation) {

    // Set size based on obstacle type
    if (type_ == ObstacleType::WALL) {
        // Walls have different dimensions based on orientation
        if (orientation_ == WallOrientation::HORIZONTAL) {
            // Horizontal walls (North/South): extend along X axis
            size_ = {WALL_LENGTH, WALL_HEIGHT, WALL_THICKNESS};
        } else {
            // Vertical walls (East/West): extend along Z axis
            size_ = {WALL_THICKNESS, WALL_HEIGHT, WALL_LENGTH};
        }
    } else if (type_ == ObstacleType::TREE) {
        // Use smaller diameter for collision detection - easier to navigate
        size_ = {TREE_COLLISION_RADIUS * 2.0f, TREE_HEIGHT, TREE_COLLISION_RADIUS * 2.0f};
    }
}

ObstacleType Obstacle::getType() const noexcept {
    return type_;
}

WallOrientation Obstacle::getOrientation() const noexcept {
    return orientation_;
}

void Obstacle::update(float deltaTime) {
    // Obstacles are static, no update needed
    // Method exists to satisfy GameObject's pure virtual function
}
