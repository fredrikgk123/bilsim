#include "core/obstacle.hpp"
#include "core/object_sizes.hpp"

Obstacle::Obstacle(float x, float y, float z, ObstacleType type, WallOrientation orientation)
    : GameObject(x, y, z), type_(type), orientation_(orientation) {

    // Set size based on obstacle type
    if (type_ == ObstacleType::WALL) {
        // Walls have different dimensions based on orientation
        if (orientation_ == WallOrientation::HORIZONTAL) {
            // Horizontal walls (North/South): extend along X axis
            size_ = {ObjectSizes::WALL_LENGTH, ObjectSizes::WALL_HEIGHT, ObjectSizes::WALL_THICKNESS};
        } else {
            // Vertical walls (East/West): extend along Z axis
            size_ = {ObjectSizes::WALL_THICKNESS, ObjectSizes::WALL_HEIGHT, ObjectSizes::WALL_LENGTH};
        }
    } else if (type_ == ObstacleType::TREE) {
        // Use smaller diameter for collision detection - easier to navigate
        size_ = {ObjectSizes::TREE_COLLISION_RADIUS * 2.0f, ObjectSizes::TREE_HEIGHT, ObjectSizes::TREE_COLLISION_RADIUS * 2.0f};
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
