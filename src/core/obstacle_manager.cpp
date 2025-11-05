#include "obstacle_manager.hpp"
#include <cmath>

namespace {
    // Wall configuration
    constexpr float WALL_HEIGHT = 2.5f;  // Height above ground
    constexpr float WALL_THICKNESS = 2.0f;  // Thickness of the continuous wall
    constexpr float WALL_SEGMENT_LENGTH = 5.0f;  // Smaller segments for smoother continuous wall

    // Tree configuration
    constexpr float TREE_HEIGHT = 0.0f;  // Trees sit on ground
    constexpr float MIN_TREE_DISTANCE_FROM_WALL = 15.0f;  // Keep trees away from walls
    constexpr float MIN_TREE_DISTANCE_FROM_CENTER = 10.0f;  // Keep trees away from spawn
    constexpr float MIN_DISTANCE_BETWEEN_TREES = 8.0f;  // Prevent trees from overlapping
}

ObstacleManager::ObstacleManager(float play_area_size, int tree_count)
    : randomEngine_(std::random_device{}()) {
    generateWalls(play_area_size);
    generateTrees(tree_count, play_area_size);
}

void ObstacleManager::generateWalls(float play_area_size) {
    const float half_size = play_area_size / 2.0f;
    const int segments_per_side = static_cast<int>(play_area_size / WALL_SEGMENT_LENGTH);

    // Generate continuous walls by placing segments with proper orientation
    for (int i = 0; i < segments_per_side; ++i) {
        float offset = -half_size + (i * WALL_SEGMENT_LENGTH) + (WALL_SEGMENT_LENGTH / 2.0f);

        // North wall (z = -half_size) - horizontal orientation
        obstacles_.push_back(std::make_unique<Obstacle>(offset, WALL_HEIGHT, -half_size, ObstacleType::WALL, WallOrientation::HORIZONTAL));

        // South wall (z = +half_size) - horizontal orientation
        obstacles_.push_back(std::make_unique<Obstacle>(offset, WALL_HEIGHT, half_size, ObstacleType::WALL, WallOrientation::HORIZONTAL));

        // West wall (x = -half_size) - vertical orientation
        obstacles_.push_back(std::make_unique<Obstacle>(-half_size, WALL_HEIGHT, offset, ObstacleType::WALL, WallOrientation::VERTICAL));

        // East wall (x = +half_size) - vertical orientation
        obstacles_.push_back(std::make_unique<Obstacle>(half_size, WALL_HEIGHT, offset, ObstacleType::WALL, WallOrientation::VERTICAL));
    }
}

void ObstacleManager::generateTrees(int count, float play_area_size) {
    const float half_size = play_area_size / 2.0f;
    const float min_pos = -half_size + MIN_TREE_DISTANCE_FROM_WALL;
    const float max_pos = half_size - MIN_TREE_DISTANCE_FROM_WALL;

    std::uniform_real_distribution<float> pos_distribution(min_pos, max_pos);

    int trees_placed = 0;
    int attempts = 0;
    const int max_attempts = count * 10;  // Prevent infinite loop

    while (trees_placed < count && attempts < max_attempts) {
        attempts++;

        float x = pos_distribution(randomEngine_);
        float z = pos_distribution(randomEngine_);

        // Check distance from center (spawn point)
        float distance_from_center = std::sqrt(x * x + z * z);
        if (distance_from_center < MIN_TREE_DISTANCE_FROM_CENTER) {
            continue;
        }

        // Check distance from other trees
        bool too_close = false;
        for (const auto& obstacle : obstacles_) {
            if (obstacle->getType() == ObstacleType::TREE) {
                const auto& pos = obstacle->getPosition();
                float distance_x = x - pos[0];
                float distance_z = z - pos[2];
                float distance = std::sqrt(distance_x * distance_x + distance_z * distance_z);

                if (distance < MIN_DISTANCE_BETWEEN_TREES) {
                    too_close = true;
                    break;
                }
            }
        }

        if (!too_close) {
            obstacles_.push_back(std::make_unique<Obstacle>(x, TREE_HEIGHT, z, ObstacleType::TREE));
            trees_placed++;
        }
    }
}

void ObstacleManager::handleCollisions(Vehicle& vehicle) const {
    const auto& vehicle_pos = vehicle.getPosition();

    // Get vehicle radius for collision (average of width and length)
    const auto& vehicle_size = vehicle.getSize();
    float vehicle_radius = (vehicle_size[0] + vehicle_size[2]) / 4.0f;

    for (const auto& obstacle : obstacles_) {
        const auto& obstacle_pos = obstacle->getPosition();
        const auto& obstacle_size = obstacle->getSize();

        // Get obstacle radius
        float obstacle_radius = (obstacle_size[0] + obstacle_size[2]) / 4.0f;

        // Calculate distance between centers
        float distance_x = vehicle_pos[0] - obstacle_pos[0];
        float distance_z = vehicle_pos[2] - obstacle_pos[2];
        float distance = std::sqrt(distance_x * distance_x + distance_z * distance_z);

        // Collision threshold = sum of radii
        float collision_threshold = vehicle_radius + obstacle_radius;

        // If distance is below threshold, we have a collision
        if (distance < collision_threshold) {
            // Calculate how much we need to push out
            float penetration = collision_threshold - distance;

            // Calculate push direction (away from obstacle)
            float push_dir_x = distance_x / distance;
            float push_dir_z = distance_z / distance;

            // Push vehicle out to exactly the threshold distance
            vehicle.setPosition(
                obstacle_pos[0] + push_dir_x * collision_threshold,
                vehicle_pos[1],
                obstacle_pos[2] + push_dir_z * collision_threshold
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
