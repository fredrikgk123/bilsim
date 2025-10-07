#include <threepp/threepp.hpp>
#include <memory>
#include <iostream>
#include <vector>
#include "core/vehicle.hpp"
#include "core/powerupManager.hpp"
#include "graphics/vehicleRenderer.hpp"
#include "graphics/powerupRenderer.hpp"
#include "graphics/sceneManager.hpp"
#include "input/inputHandler.hpp"
#include "audio/audioManager.hpp"
#include "ui/uiManager.hpp"

using namespace threepp;

int main() {
    // Game constants
    const int POWERUP_COUNT = 20;
    const float PLAY_AREA_SIZE = 200.0f;

    // UI Constants
    int minimapSize = 150; // Minimap size in pixels
    int minimapPadding = 10; // Padding from screen corner

    Canvas canvas("Bilsimulator");

    // Initialize scene
    SceneManager sceneManager;
    sceneManager.setupCamera(canvas.aspect());
    sceneManager.setupMinimapCamera(1.0f); // 1.0 aspect ratio - square minimap
    sceneManager.setupRenderer(canvas.size());
    sceneManager.setupLighting();
    sceneManager.setupGround();

    // Create vehicle and renderer
    Vehicle vehicle(0.0f, 0.0f, 0.0f);
    VehicleRenderer vehicleRenderer(sceneManager.getScene(), vehicle);

    // Create powerup manager with 20 randomly placed powerups
    PowerupManager powerupManager(POWERUP_COUNT, PLAY_AREA_SIZE);

    // Create renderers for all powerups
    std::vector<std::unique_ptr<PowerupRenderer>> powerupRenderers;
    for (const auto& powerup : powerupManager.getPowerups()) {
        auto renderer = std::make_unique<PowerupRenderer>(sceneManager.getScene(), *powerup);
        powerupRenderers.push_back(std::move(renderer));
    }

    // Setup input handling
    std::unique_ptr<InputHandler> inputHandler = std::make_unique<InputHandler>(vehicle);
    canvas.addKeyListener(*inputHandler);

    // Set up reset callback to respawn powerups
    inputHandler->setResetCallback([&powerupManager]() {
        powerupManager.reset();
    });

    // Initialize audio (optional)
    AudioManager audioManager;
    bool audioEnabled = audioManager.initialize("carnoise.wav");

    if (audioEnabled == false) {
        std::cout << "Audio file 'carnoise.wav' not found. Continuing without audio..." << std::endl;
    }

    // Setup UI
    UIManager uiManager(sceneManager.getRenderer());

    // Handle window resize
    canvas.onWindowResize([&](const WindowSize& size) {
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

        // Update powerups and handle collisions
        powerupManager.update(deltaTime);
        powerupManager.handleCollisions(vehicle);

        // Update powerup renderers
        for (auto& renderer : powerupRenderers) {
            renderer->update();
        }

        // Update camera to follow vehicle
        std::array<float, 3> position = vehicle.getPosition();
        float rotation = vehicle.getRotation();
        sceneManager.updateCameraFollowTarget(position[0], position[1], position[2], rotation);
        sceneManager.updateMinimapCamera(position[0], position[2]);

        // Update camera FOV based on nitrous state (speed FOV effect)
        sceneManager.updateCameraFOV(vehicle.isNitrousActive());

        // Update audio
        if (audioEnabled == true) {
            audioManager.update(vehicle);
        }

        GLRenderer &renderer = sceneManager.getRenderer();
        const WindowSize& size = canvas.size();

        // Render main view
        renderer.setViewport(0, 0, size.width(), size.height());
        renderer.setScissor(0, 0, size.width(), size.height());
        renderer.setScissorTest(false);
        sceneManager.getRenderer().render(sceneManager.getScene(), sceneManager.getCamera());

        // Render minimap
        int minimapX = minimapPadding;
        int minimapY = size.height() - minimapSize - minimapPadding;
        renderer.setViewport(minimapX, minimapY, minimapSize, minimapSize);
        renderer.setScissor(minimapX, minimapY, minimapSize, minimapSize);
        renderer.setScissorTest(true);
        sceneManager.renderMinimap();
        renderer.setScissorTest(false);

        // Render UI overlay
        renderer.setViewport(0, 0, size.width(), size.height());
        uiManager.render(vehicle, size);
    });

    return 0;
}
