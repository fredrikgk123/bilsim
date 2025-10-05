#include <threepp/threepp.hpp>
#include <memory>
#include <filesystem>
#include <iostream>
#include "vehicle.hpp"
#include "vehicleRenderer.hpp"
#include "sceneManager.hpp"
#include "inputHandler.hpp"
#include "audioManager.hpp"

using namespace threepp;

int main() {
    // Create window
    Canvas canvas("Bilsimulator");

    // Create scene manager
    SceneManager sceneManager;
    sceneManager.setupCamera(canvas.aspect());
    sceneManager.setupRenderer(canvas.size());
    sceneManager.setupLighting();
    sceneManager.setupGround();

    // Create vehicle (backend - pure math)
    Vehicle vehicle(0.0f, 0.0f, 0.0f);

    // Create vehicle renderer (frontend - visualization)
    VehicleRenderer vehicleRenderer(sceneManager, vehicle);

    // Create input handler
    auto inputHandler = std::make_unique<InputHandler>(vehicle);
    canvas.addKeyListener(*inputHandler);

    // Create audio manager and initialize engine sound
    AudioManager audioManager;
    bool audioEnabled = audioManager.initialize("carnoise.wav");

    if (!audioEnabled) {
        std::cout << "Audio file 'carnoise.wav' not found. Continuing without audio..." << std::endl;
    }

    // Handle window resize
    canvas.onWindowResize([&](WindowSize size) {
        sceneManager.resize(size);
    });

    // Animation loop
    Clock clock;
    canvas.animate([&] {
        float deltaTime = clock.getDelta();

        // Update input and vehicle physics
        inputHandler->update(deltaTime);
        vehicle.update(deltaTime);

        // Update renderer
        vehicleRenderer.update();

        // Update camera to follow vehicle
        auto pos = vehicle.getPosition();
        sceneManager.updateCameraFollowTarget(pos[0], pos[1], pos[2], vehicle.getRotation());

        // Update audio based on vehicle state
        if (audioEnabled) {
            audioManager.update(vehicle);
        }

        // Render the scene
        sceneManager.getRenderer().render(sceneManager.getScene(), sceneManager.getCamera());
    });

    return 0;
}
