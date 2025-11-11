#include "obstacle_manager.hpp"
#include "game_config.hpp"
#include "random_position_generator.hpp"
#include <cmath>


ObstacleManager::ObstacleManager(float play_area_size, int tree_count) {
    generateWalls(play_area_size);
    generateTrees(tree_count, play_area_size);
}

void ObstacleManager::update(float deltaTime) {
    // Obstacles are static, no update needed
}

void ObstacleManager::generateWalls(float play_area_size) {
    const float half_size = play_area_size / 2.0f;
    const int segments_per_side = static_cast<int>(play_area_size / GameConfig::Obstacle::WALL_SEGMENT_LENGTH);

    // Generate continuous walls by placing segments with proper orientation
    for (int i = 0; i < segments_per_side; ++i) {
        float offset = -half_size + (static_cast<float>(i) * GameConfig::Obstacle::WALL_SEGMENT_LENGTH) + (GameConfig::Obstacle::WALL_SEGMENT_LENGTH / 2.0f);

        // North wall (z = -half_size) - horizontal orientation
        obstacles_.push_back(std::make_unique<Obstacle>(offset, GameConfig::Obstacle::WALL_HEIGHT, -half_size, ObstacleType::WALL, WallOrientation::HORIZONTAL));

        // South wall (z = +half_size) - horizontal orientation
        obstacles_.push_back(std::make_unique<Obstacle>(offset, GameConfig::Obstacle::WALL_HEIGHT, half_size, ObstacleType::WALL, WallOrientation::HORIZONTAL));

        // West wall (x = -half_size) - vertical orientation
        obstacles_.push_back(std::make_unique<Obstacle>(-half_size, GameConfig::Obstacle::WALL_HEIGHT, offset, ObstacleType::WALL, WallOrientation::VERTICAL));

        // East wall (x = +half_size) - vertical orientation
        obstacles_.push_back(std::make_unique<Obstacle>(half_size, GameConfig::Obstacle::WALL_HEIGHT, offset, ObstacleType::WALL, WallOrientation::VERTICAL));
    }
}

void ObstacleManager::generateTrees(int count, float play_area_size) {
    // Create position generator with margin from walls
    RandomPositionGenerator posGen(play_area_size, GameConfig::Obstacle::MIN_TREE_DISTANCE_FROM_WALL);

    // Track existing tree positions for spacing validation
    std::vector<std::array<float, 2>> tree_positions;

    int trees_placed = 0;
    int total_attempts = 0;
    const int max_total_attempts = count * 20;  // More attempts to find valid positions

    while (trees_placed < count && total_attempts < max_total_attempts) {
        total_attempts++;

        // Get a random position
        auto pos = posGen.getRandomPosition();

        // Check distance from center (spawn point)
        float distance_from_center = std::sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
        if (distance_from_center < GameConfig::Obstacle::MIN_TREE_DISTANCE_FROM_CENTER) {
            continue; // Too close to center, try again
        }

        // Check distance from other trees
        bool valid_position = true;
        for (const auto& existing_pos : tree_positions) {
            float dx = pos[0] - existing_pos[0];
            float dz = pos[1] - existing_pos[1];
            float distance = std::sqrt(dx * dx + dz * dz);

            if (distance < GameConfig::Obstacle::MIN_DISTANCE_BETWEEN_TREES) {
                valid_position = false;
                break;
            }
        }

        if (valid_position) {
            // Create tree at validated position
            obstacles_.push_back(std::make_unique<Obstacle>(
                pos[0],
                GameConfig::Obstacle::TREE_HEIGHT,
                pos[1],
                ObstacleType::TREE
            ));
            tree_positions.push_back(pos);
            trees_placed++;
        }
    }
}

void ObstacleManager::handleCollisions(Vehicle& vehicle) {
    for (const auto& obstacle : obstacles_) {
        float overlap_distance, normal_x, normal_z;

        // Check for AABB collision
        if (vehicle.checkCircleCollision(*obstacle, overlap_distance, normal_x, normal_z)) {
            // Push vehicle out along the collision normal
            const auto& vehicle_pos = vehicle.getPosition();
            vehicle.setPosition(
                vehicle_pos[0] - normal_x * overlap_distance,
                vehicle_pos[1],
                vehicle_pos[2] - normal_z * overlap_distance
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
