#pragma once

#include "graphics/game_object_renderer.hpp"
#include "core/obstacle.hpp"

/**
 * ObstacleRenderer - renders obstacles (walls and trees) in the scene
 */
class ObstacleRenderer : public GameObjectRenderer {
public:
    ObstacleRenderer(threepp::Scene& scene, const Obstacle& obstacle);

    // Override update to handle obstacle-specific behavior
    void update() override;

protected:
    // Override to create obstacle-specific models
    void createModel() override;

private:
    void createWallMesh();
    void createTreeMesh();

    const Obstacle& obstacle_;
};

