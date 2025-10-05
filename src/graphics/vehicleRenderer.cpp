#include "vehicleRenderer.hpp"
#include <cmath>

using namespace threepp;

VehicleRenderer::VehicleRenderer(SceneManager& sceneManager, const Vehicle& vehicle)
    : sceneManager_(sceneManager),
      vehicle_(vehicle) {

    // Create a group for the vehicle (allows complex models with multiple parts in the future)
    vehicleGroup_ = std::make_shared<Group>();
    sceneManager_.getScene().add(vehicleGroup_);

    createVehicleModel();
}

VehicleRenderer::~VehicleRenderer() {
    // Clean up: remove from scene when renderer is destroyed
    if (vehicleGroup_) {
        sceneManager_.getScene().remove(*vehicleGroup_);
    }
}

void VehicleRenderer::createVehicleModel() {
    // Get vehicle dimensions from physics model
    auto size = vehicle_.getSize();

    // Create simple box geometry for now (can be replaced with complex model later)
    auto geometry = BoxGeometry::create(size[0], size[1], size[2]);
    auto material = MeshPhongMaterial::create();  // Phong material supports lighting and shadows
    material->color = Color::red;

    bodyMesh_ = Mesh::create(geometry, material);
    bodyMesh_->position.y = size[1] / 2;  // Adjust position so bottom is at y=0 (vehicle sits on ground)
    bodyMesh_->castShadow = true;

    vehicleGroup_->add(bodyMesh_);
}

void VehicleRenderer::update() {
    // Sync visual representation with physics model
    auto position = vehicle_.getPosition();
    vehicleGroup_->position.set(position[0], position[1], position[2]);
    vehicleGroup_->rotation.y = vehicle_.getRotation();
}

void VehicleRenderer::setColor(const Color& color) {
    if (bodyMesh_ && vehicleGroup_) {
        // Rebuild mesh with new color (simple approach for color changes)
        auto size = vehicle_.getSize();

        vehicleGroup_->remove(*bodyMesh_);

        auto geometry = BoxGeometry::create(size[0], size[1], size[2]);
        auto material = MeshPhongMaterial::create();
        material->color = color;

        bodyMesh_ = Mesh::create(geometry, material);
        bodyMesh_->position.y = size[1] / 2;
        bodyMesh_->castShadow = true;

        vehicleGroup_->add(bodyMesh_);
    }
}
