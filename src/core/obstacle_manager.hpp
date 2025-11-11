#pragma once

#include "obstacle.hpp"
#include "vehicle.hpp"
#include "game_object_manager.hpp"
#include "random_position_generator.hpp"
#include <vector>
#include <memory>

/**
 * ObstacleManager - manages all obstacles in the scene
 * Creates perimeter walls and scattered trees
 */
class ObstacleManager : public GameObjectManager {
public:
    ObstacleManager(float playAreaSize, int treeCount);

    // Update all obstacles (static, so no-op)
    virtual void update(float deltaTime) override;

    // Collision detection with vehicle
    virtual void handleCollisions(Vehicle& vehicle) override;

    // Access to obstacles for rendering
    [[nodiscard]] const std::vector<std::unique_ptr<Obstacle>>& getObstacles() const noexcept;

    // Reset is not needed for static obstacles, but included for consistency
    virtual void reset() noexcept override;

    // Get count of obstacles
    virtual size_t getCount() const override;

private:
    void generateWalls(float playAreaSize);
    void generateTrees(int count, float playAreaSize);

    std::vector<std::unique_ptr<Obstacle>> obstacles_;
};
