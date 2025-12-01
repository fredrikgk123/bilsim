#include "graphics/obstacle_renderer.hpp"
#include "core/game_config.hpp"

using namespace threepp;

namespace {
    // Wall visual constants (for rendering - may differ from collision sizes) //not happy with this solution
    constexpr float WALL_WIDTH = 5.0f;
    constexpr float WALL_HEIGHT = 5.0f;  // Visual height for rendering
    constexpr float WALL_DEPTH = 2.0f;
    constexpr unsigned int WALL_COLOR = 0x8B4513;

    // Tree visual constants
    constexpr float TREE_TRUNK_RADIUS = 0.4f;
    constexpr float TREE_TRUNK_HEIGHT = 3.0f;
    constexpr float TREE_FOLIAGE_RADIUS = 2.0f;
    constexpr unsigned int TRUNK_COLOR = 0x8B4513;
    constexpr unsigned int FOLIAGE_COLOR = 0x228B22;
}

ObstacleRenderer::ObstacleRenderer(Scene& scene, const Obstacle& obstacle)
    : GameObjectRenderer(scene, obstacle), obstacle_(obstacle) {
    createModel();
}

void ObstacleRenderer::createModel() {
    // Create appropriate mesh based on obstacle type
    if (obstacle_.getType() == ObstacleType::WALL) {
        createWallMesh();
    } else if (obstacle_.getType() == ObstacleType::TREE) {
        createTreeMesh();
    }
}

void ObstacleRenderer::createWallMesh() {
    // Get wall orientation to create proper dimensions
    auto orientation = obstacle_.getOrientation();

    // Create box with dimensions based on orientation
    std::shared_ptr<threepp::BoxGeometry> geometry;
    if (orientation == WallOrientation::HORIZONTAL) {
        // Horizontal walls (North/South): extend along X axis
        geometry = BoxGeometry::create(WALL_WIDTH, WALL_HEIGHT, WALL_DEPTH);
    } else {
        // Vertical walls (East/West): extend along Z axis
        geometry = BoxGeometry::create(WALL_DEPTH, WALL_HEIGHT, WALL_WIDTH);
    }

    auto material = MeshPhongMaterial::create();
    material->color = Color(WALL_COLOR);

    auto wallMesh = Mesh::create(geometry, material);
    wallMesh->castShadow = true;
    wallMesh->receiveShadow = true;

    objectGroup_->add(wallMesh);
}

void ObstacleRenderer::createTreeMesh() {
    // Create trunk (cylinder)
    auto trunkGeometry = CylinderGeometry::create(TREE_TRUNK_RADIUS, TREE_TRUNK_RADIUS, TREE_TRUNK_HEIGHT);
    auto trunkMaterial = MeshPhongMaterial::create();
    trunkMaterial->color = Color(TRUNK_COLOR);

    auto trunkMesh = Mesh::create(trunkGeometry, trunkMaterial);
    trunkMesh->position.y = TREE_TRUNK_HEIGHT / 2.0f;
    trunkMesh->castShadow = true;
    trunkMesh->receiveShadow = true;

    // Create foliage (sphere)
    auto foliageGeometry = SphereGeometry::create(TREE_FOLIAGE_RADIUS);
    auto foliageMaterial = MeshPhongMaterial::create();
    foliageMaterial->color = Color(FOLIAGE_COLOR);

    auto foliageMesh = Mesh::create(foliageGeometry, foliageMaterial);
    foliageMesh->position.y = TREE_TRUNK_HEIGHT + TREE_FOLIAGE_RADIUS * 0.5f;
    foliageMesh->castShadow = true;
    foliageMesh->receiveShadow = true;

    objectGroup_->add(trunkMesh);
    objectGroup_->add(foliageMesh);
}

void ObstacleRenderer::update() {
    // Obstacles are static, but we still call base update for consistency
    GameObjectRenderer::update();
}
