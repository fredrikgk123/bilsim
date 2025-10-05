#pragma once

#include <threepp/threepp.hpp>
#include "vehicle.hpp"
#include "sceneManager.hpp"

class VehicleRenderer {

  public:
    VehicleRenderer(SceneManager& sceneManager, const Vehicle& vehicle);
    ~VehicleRenderer();

    void update();
    void setColor(const threepp::Color& color);

  private:
    void createVehicleModel();

    SceneManager& sceneManager_;
    const Vehicle& vehicle_;
    std::shared_ptr<threepp::Group> vehicleGroup_;
    std::shared_ptr<threepp::Mesh> bodyMesh_;
};
