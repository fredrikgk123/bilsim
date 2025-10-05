#include <threepp/threepp.hpp>
#include <memory>
#include <iostream>
#include "vehicle.hpp"
#include "vehicleRenderer.hpp"
#include "sceneManager.hpp"
#include "inputHandler.hpp"
#include "audioManager.hpp"
#include "uiManager.hpp"

using namespace threepp;

int main() {
    Canvas canvas("Bilsimulator");

    // Initialize scene
    SceneManager sceneManager;
    sceneManager.setupCamera(canvas.aspect());
    sceneManager.setupMinimapCamera(1.0f);  // 1.0 aspect ratio - square minimap
    sceneManager.setupRenderer(canvas.size());
    sceneManager.setupLighting();
    sceneManager.setupGround();

    // Create vehicle and renderer
    Vehicle vehicle(0.0f, 0.0f, 0.0f);
    VehicleRenderer vehicleRenderer(sceneManager, vehicle);

    // Setup input handling
    auto inputHandler = std::make_unique<InputHandler>(vehicle);
    canvas.addKeyListener(*inputHandler);

    // Initialize audio (optional)
    AudioManager audioManager;
    bool audioEnabled = audioManager.initialize("carnoise.wav");

    if (!audioEnabled) {
        std::cout << "Audio file 'carnoise.wav' not found. Continuing without audio..." << std::endl;
    }

    // Setup UI
    UIManager uiManager(sceneManager.getRenderer());

    // Handle window resize
    canvas.onWindowResize([&](WindowSize size) {
        sceneManager.resize(size);
    });

    // Main game loop
    Clock clock;
    canvas.animate([&] {
        float deltaTime = clock.getDelta();

        // Update game state
        inputHandler->update(deltaTime);
        vehicle.update(deltaTime);
        vehicleRenderer.update();

        // Update camera to follow vehicle
        auto pos = vehicle.getPosition();
        sceneManager.updateCameraFollowTarget(pos[0], pos[1], pos[2], vehicle.getRotation());
        sceneManager.updateMinimapCamera(pos[0], pos[2]);

        // Update audio
        if (audioEnabled) {
            audioManager.update(vehicle);
        }

        auto& renderer = sceneManager.getRenderer();
        auto size = canvas.size();

        // Render main view
        renderer.setViewport(0, 0, size.width(), size.height());
        renderer.setScissor(0, 0, size.width(), size.height());
        renderer.setScissorTest(false);
        sceneManager.getRenderer().render(sceneManager.getScene(), sceneManager.getCamera());

        // Render minimap
        int minimapSize = 150;  // 150 pixels - small enough to not obstruct view, large enough to be useful
        renderer.setViewport(10, size.height() - minimapSize - 10, minimapSize, minimapSize);  // 10px padding from corner
        renderer.setScissor(10, size.height() - minimapSize - 10, minimapSize, minimapSize);
        renderer.setScissorTest(true);
        sceneManager.renderMinimap();
        renderer.setScissorTest(false);

        // Render UI overlay
        renderer.setViewport(0, 0, size.width(), size.height());
        uiManager.render(vehicle, size);
    });

    return 0;
}
