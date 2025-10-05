#include "vehicleRenderer.hpp"

using namespace threepp;

VehicleRenderer::VehicleRenderer(SceneManager& sceneManager, const Vehicle& vehicle)
    : sceneManager_(sceneManager),
      vehicle_(vehicle) {

    vehicleGroup_ = std::make_shared<Group>();
    sceneManager_.getScene().add(vehicleGroup_);

    createVehicleModel();
}

VehicleRenderer::~VehicleRenderer() {
    if (vehicleGroup_) {
        sceneManager_.getScene().remove(*vehicleGroup_);
    }
}

void VehicleRenderer::createVehicleModel() {
    auto size = vehicle_.getSize();

    // Create simple box geometry for vehicle
    auto geometry = BoxGeometry::create(size[0], size[1], size[2]);
    auto material = MeshPhongMaterial::create();
    material->color = Color::red;

    bodyMesh_ = Mesh::create(geometry, material);
    bodyMesh_->position.y = size[1] / 2;  // Half height - positions box so bottom sits at y=0 (on ground)
    bodyMesh_->castShadow = true;

    vehicleGroup_->add(bodyMesh_);
}

void VehicleRenderer::update() {
    // Sync visual representation with physics model
    auto position = vehicle_.getPosition();
    vehicleGroup_->position.set(position[0], position[1], position[2]);
    vehicleGroup_->rotation.y = vehicle_.getRotation();
}
