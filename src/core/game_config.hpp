#pragma once

/**
 * Centralized game configuration constants
 * All magic numbers and tunable parameters should be defined here
 */

namespace GameConfig {

// World configuration
namespace World {
    inline constexpr float PLAY_AREA_SIZE = 200.0f;
    inline constexpr float SPAWN_POINT_X = 0.0f;
    inline constexpr float SPAWN_POINT_Y = 0.0f;
    inline constexpr float SPAWN_POINT_Z = 0.0f;
}

// Powerup spawning configuration
namespace Powerup {
    inline constexpr int DEFAULT_COUNT = 20;
    inline constexpr float SPAWN_MARGIN = 10.0f;  // Distance from play area edges
    inline constexpr float HEIGHT = 0.4f;         // Fixed height above ground
}

// Obstacle spawning configuration
namespace Obstacle {
    inline constexpr int DEFAULT_TREE_COUNT = 30;
    inline constexpr float TREE_HEIGHT = 0.0f;  // Trees sit on ground
    inline constexpr float MIN_TREE_DISTANCE_FROM_WALL = 15.0f;
    inline constexpr float MIN_TREE_DISTANCE_FROM_CENTER = 10.0f;
    inline constexpr float MIN_DISTANCE_BETWEEN_TREES = 8.0f;

    // Wall configuration
    inline constexpr float WALL_HEIGHT = 2.5f;
    inline constexpr float WALL_THICKNESS = 2.0f;
    inline constexpr float WALL_SEGMENT_LENGTH = 5.0f;
}

// UI configuration
namespace UI {
    inline constexpr int MINIMAP_SIZE = 150;
    inline constexpr int MINIMAP_PADDING = 10;
    inline constexpr float MINIMAP_ASPECT_RATIO = 1.0f;
}

// Asset paths
namespace Assets {
    inline constexpr const char* CAR_MODEL_PATH = "assets/body.obj";
    inline constexpr const char* ENGINE_SOUND_PATH = "assets/carnoise.wav";
    inline constexpr const char* DRIFT_SOUND_PATH = "assets/tireScreech.wav";
}

} // namespace GameConfig

