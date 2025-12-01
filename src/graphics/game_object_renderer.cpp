#include "graphics/game_object_renderer.hpp"
#include "core/game_object.hpp"

using namespace threepp;


GameObjectRenderer::GameObjectRenderer(Scene &scene, const GameObject &gameObject)
    : scene_(scene),
      gameObject_(gameObject) {
    objectGroup_ = std::make_shared<Group>();
    scene_.add(objectGroup_);
}

GameObjectRenderer::~GameObjectRenderer() {
    if (objectGroup_) {
        scene_.remove(*objectGroup_);
    }
}

void GameObjectRenderer::createModel() {
    std::array<float, 3> size = gameObject_.getSize();

    // Create simple box geometry by default
    auto geometry = BoxGeometry::create(size[0], size[1], size[2]);
    auto material = MeshPhongMaterial::create();
    material->color = Color::white;

    bodyMesh_ = Mesh::create(geometry, material);
    bodyMesh_->position.y = size[1] / 2.0f; // Half height - positions box so bottom sits at y=0
    bodyMesh_->castShadow = true;

    objectGroup_->add(bodyMesh_);
}

void GameObjectRenderer::update() {
    // Sync visual representation with game object
    std::array<float, 3> position = gameObject_.getPosition();
    objectGroup_->position.set(position[0], position[1], position[2]);
    objectGroup_->rotation.y = gameObject_.getRotation();

    // Handle active/inactive state
    objectGroup_->visible = gameObject_.isActive();
}

void GameObjectRenderer::setVisible(bool visible) {
    if (objectGroup_) {
        objectGroup_->visible = visible;
    }
}
