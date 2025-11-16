#include "graphics/powerup_renderer.hpp"

using namespace threepp;

namespace {
    constexpr float CYLINDER_RADIUS_RATIO = 0.3f;    // Powerup cylinder radius as ratio of size
    constexpr int CYLINDER_RADIAL_SEGMENTS = 8;      // Number of segments in cylinder (octagon)
    constexpr unsigned int NITROUS_COLOR = 0x00aaff;  // Bright blue
    constexpr unsigned int NITROUS_EMISSIVE = 0x0066aa; // Glowing effect color
    constexpr float NITROUS_EMISSIVE_INTENSITY = 0.5f;
}

PowerupRenderer::PowerupRenderer(Scene& scene, const Powerup& powerup)
    : GameObjectRenderer(scene, powerup),
      powerup_(powerup) {
    createModel();
}

void PowerupRenderer::createModel() {
    std::array<float, 3> size = powerup_.getSize();

    // Create a distinctive visual for nitrous - blue glowing cylinder
    auto geometry = CylinderGeometry::create(
        size[0] * CYLINDER_RADIUS_RATIO,
        size[0] * CYLINDER_RADIUS_RATIO,
        size[1],
        CYLINDER_RADIAL_SEGMENTS
    );
    auto material = MeshPhongMaterial::create();

    if (powerup_.getType() == PowerupType::NITROUS) {
        material->color = Color(NITROUS_COLOR);
        material->emissive = Color(NITROUS_EMISSIVE);
        material->emissiveIntensity = NITROUS_EMISSIVE_INTENSITY;
    }

    bodyMesh_ = Mesh::create(geometry, material);
    bodyMesh_->position.y = size[1] / 2.0f;
    bodyMesh_->castShadow = true;

    objectGroup_->add(bodyMesh_);
}
