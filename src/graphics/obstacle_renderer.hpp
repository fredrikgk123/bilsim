#pragma once

#include <threepp/threepp.hpp>
#include <memory>
#include "../core/obstacle.hpp"

/**
 * ObstacleRenderer - renders obstacles (walls and trees) in the scene
 */
class ObstacleRenderer {
public:
    ObstacleRenderer(threepp::Scene& scene, const Obstacle& obstacle);
    ~ObstacleRenderer() = default;

    void update();

private:
    void createWallMesh();
    void createTreeMesh();

    const Obstacle& obstacle_;
    threepp::Scene& scene_;
    std::shared_ptr<threepp::Group> obstacleGroup_;
};

