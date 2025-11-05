#pragma once

#include "obstacle.hpp"
#include "vehicle.hpp"
#include <vector>
#include <memory>
#include <random>

/**
 * ObstacleManager - manages all obstacles in the scene
 * Creates perimeter walls and scattered trees
 */
class ObstacleManager {
public:
    ObstacleManager(float playAreaSize, int treeCount);

    // Collision detection with vehicle
    void handleCollisions(Vehicle& vehicle) const;

    // Access to obstacles for rendering
    [[nodiscard]] const std::vector<std::unique_ptr<Obstacle>>& getObstacles() const noexcept;

    // Reset is not needed for static obstacles, but included for consistency
    void reset() noexcept;

private:
    void generateWalls(float playAreaSize);
    void generateTrees(int count, float playAreaSize);

    std::vector<std::unique_ptr<Obstacle>> obstacles_;
    std::mt19937 randomEngine_;
};
