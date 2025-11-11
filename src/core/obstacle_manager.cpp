#include "obstacle_manager.hpp"
#include "game_config.hpp"
#include "random_position_generator.hpp"
#include <cmath>


ObstacleManager::ObstacleManager(float playAreaSize, int treeCount) {
    generateWalls(playAreaSize);
    generateTrees(treeCount, playAreaSize);
}

void ObstacleManager::update(float deltaTime) {
    // Obstacles are static, no update needed
}

void ObstacleManager::generateWalls(float playAreaSize) {
    const float halfSize = playAreaSize / 2.0f;
    const int segmentsPerSide = static_cast<int>(playAreaSize / GameConfig::Obstacle::WALL_SEGMENT_LENGTH);

    // Generate continuous walls by placing segments with proper orientation
    for (int i = 0; i < segmentsPerSide; ++i) {
        float offset = -halfSize + (static_cast<float>(i) * GameConfig::Obstacle::WALL_SEGMENT_LENGTH) + (GameConfig::Obstacle::WALL_SEGMENT_LENGTH / 2.0f);

        // North wall (z = -halfSize) - horizontal orientation
        obstacles_.push_back(std::make_unique<Obstacle>(offset, GameConfig::Obstacle::WALL_HEIGHT, -halfSize, ObstacleType::WALL, WallOrientation::HORIZONTAL));

        // South wall (z = +halfSize) - horizontal orientation
        obstacles_.push_back(std::make_unique<Obstacle>(offset, GameConfig::Obstacle::WALL_HEIGHT, halfSize, ObstacleType::WALL, WallOrientation::HORIZONTAL));

        // West wall (x = -halfSize) - vertical orientation
        obstacles_.push_back(std::make_unique<Obstacle>(-halfSize, GameConfig::Obstacle::WALL_HEIGHT, offset, ObstacleType::WALL, WallOrientation::VERTICAL));

        // East wall (x = +halfSize) - vertical orientation
        obstacles_.push_back(std::make_unique<Obstacle>(halfSize, GameConfig::Obstacle::WALL_HEIGHT, offset, ObstacleType::WALL, WallOrientation::VERTICAL));
    }
}

void ObstacleManager::generateTrees(int count, float playAreaSize) {
    // Create position generator with margin from walls
    RandomPositionGenerator posGen(playAreaSize, GameConfig::Obstacle::MIN_TREE_DISTANCE_FROM_WALL);

    // Track existing tree positions for spacing validation
    std::vector<std::array<float, 2>> treePositions;

    int treesPlaced = 0;
    int totalAttempts = 0;
    const int maxTotalAttempts = count * 20;  // More attempts to find valid positions

    while (treesPlaced < count && totalAttempts < maxTotalAttempts) {
        totalAttempts++;

        // Get a random position
        auto pos = posGen.getRandomPosition();

        // Check distance from center (spawn point)
        float distanceFromCenter = std::sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
        if (distanceFromCenter < GameConfig::Obstacle::MIN_TREE_DISTANCE_FROM_CENTER) {
            continue; // Too close to center, try again
        }

        // Check distance from other trees
        bool validPosition = true;
        for (const auto& existingPos : treePositions) {
            float dx = pos[0] - existingPos[0];
            float dz = pos[1] - existingPos[1];
            float distance = std::sqrt(dx * dx + dz * dz);

            if (distance < GameConfig::Obstacle::MIN_DISTANCE_BETWEEN_TREES) {
                validPosition = false;
                break;
            }
        }

        if (validPosition) {
            // Create tree at validated position
            obstacles_.push_back(std::make_unique<Obstacle>(
                pos[0],
                GameConfig::Obstacle::TREE_HEIGHT,
                pos[1],
                ObstacleType::TREE
            ));
            treePositions.push_back(pos);
            treesPlaced++;
        }
    }
}

void ObstacleManager::handleCollisions(Vehicle& vehicle) {
    for (const auto& obstacle : obstacles_) {
        float overlapDistance, normalX, normalZ;

        if (vehicle.checkCircleCollision(*obstacle, overlapDistance, normalX, normalZ)) {
            // Push vehicle out along the collision normal
            const auto& vehiclePos = vehicle.getPosition();
            vehicle.setPosition(
                vehiclePos[0] - normalX * overlapDistance,
                vehiclePos[1],
                vehiclePos[2] - normalZ * overlapDistance
            );

            // Stop the vehicle
            vehicle.setVelocity(0.0f);

            // Only process first collision per frame
            break;
        }
    }
}

void ObstacleManager::reset() noexcept {
    // Obstacles are static, no reset needed
    // But method exists for consistency with other managers
}

const std::vector<std::unique_ptr<Obstacle>>& ObstacleManager::getObstacles() const noexcept {
    return obstacles_;
}

size_t ObstacleManager::getCount() const {
    return obstacles_.size();
}
