#include "powerupRenderer.hpp"

using namespace threepp;

PowerupRenderer::PowerupRenderer(Scene& scene, const Powerup& powerup)
    : GameObjectRenderer(scene, powerup),
      powerup_(powerup) {
    createModel();
}

void PowerupRenderer::createModel() {
    std::array<float, 3> size = powerup_.getSize();

    // Create a distinctive visual for nitrous - blue glowing cylinder
    auto geometry = CylinderGeometry::create(size[0] * 0.3f, size[0] * 0.3f, size[1], 8);
    auto material = MeshPhongMaterial::create();

    if (powerup_.getType() == PowerupType::NITROUS) {
        material->color = Color(0x00aaff); // Bright blue
        material->emissive = Color(0x0066aa); // Glowing effect
        material->emissiveIntensity = 0.5f;
    }

    bodyMesh_ = Mesh::create(geometry, material);
    bodyMesh_->position.y = size[1] / 2.0f;
    bodyMesh_->castShadow = true;

    objectGroup_->add(bodyMesh_);
}
