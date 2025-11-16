#pragma once

#include "game_object.hpp"

enum class ObstacleType {
    WALL,
    TREE
};

enum class WallOrientation {
    HORIZONTAL,  // North/South walls (extends along X axis)
    VERTICAL     // East/West walls (extends along Z axis)
};

/**
 * Obstacle class - represents static obstacles in the environment
 * Walls form the perimeter, trees are scattered throughout
 */
class Obstacle : public GameObject {
public:
    Obstacle(float x, float y, float z, ObstacleType type, WallOrientation orientation = WallOrientation::HORIZONTAL);

    [[nodiscard]] ObstacleType getType() const noexcept;
    [[nodiscard]] WallOrientation getOrientation() const noexcept;

    // Override from GameObject - obstacles are static, no update needed
    void update(float deltaTime) override;

private:
    ObstacleType type_;
    WallOrientation orientation_;
};
