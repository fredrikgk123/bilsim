#include <threepp/threepp.hpp>
#include <memory>
#include <iostream>
#include <vector>
#include "core/vehicle.hpp"
#include "core/powerup.hpp"
#include "graphics/vehicleRenderer.hpp"
#include "graphics/powerupRenderer.hpp"
#include "graphics/sceneManager.hpp"
#include "input/inputHandler.hpp"
#include "audio/audioManager.hpp"
#include "ui/uiManager.hpp"

using namespace threepp;

int main() {
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

    // Create powerups scattered around the scene
    std::vector<std::unique_ptr<Powerup>> powerups;
    std::vector<std::unique_ptr<PowerupRenderer>> powerupRenderers;

    // Hardcoded powerup positions - scattered around the play area
    std::vector<std::array<float, 2>> powerupPositions = {
        {10.0f, 10.0f},
        {-15.0f, 8.0f},
        {20.0f, -12.0f},
        {-8.0f, -20.0f},
        {25.0f, 5.0f},
        {-18.0f, 18.0f},
        {5.0f, -25.0f},
        {-25.0f, -5.0f}
    };

    for (const auto& pos : powerupPositions) {
        auto powerup = std::make_unique<Powerup>(pos[0], 0.4f, pos[1], PowerupType::NITROUS);
        auto renderer = std::make_unique<PowerupRenderer>(sceneManager.getScene(), *powerup);
        powerups.push_back(std::move(powerup));
        powerupRenderers.push_back(std::move(renderer));
    }

    // Setup input handling
    std::unique_ptr<InputHandler> inputHandler = std::make_unique<InputHandler>(vehicle);
    canvas.addKeyListener(*inputHandler);

    // Set up reset callback to respawn powerups
    inputHandler->setResetCallback([&powerups]() {
        for (size_t i = 0; i < powerups.size(); i = i + 1) {
            powerups[i]->reset();
        }
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

        // Update powerups
        for (size_t i = 0; i < powerups.size(); i = i + 1) {
            powerups[i]->update(deltaTime);
            powerupRenderers[i]->update();

            // Check collision with vehicle
            // Only allow pickup if:
            // 1. Powerup is active (not already collected)
            // 2. Vehicle doesn't have nitrous stored
            // 3. Vehicle is not currently using nitrous
            // 4. Vehicle is colliding with the powerup
            if (powerups[i]->isActive() == true &&
                vehicle.hasNitrous() == false &&
                vehicle.isNitrousActive() == false &&
                vehicle.intersects(*powerups[i]) == true) {
                vehicle.pickupNitrous();
                powerups[i]->setActive(false);
            }
        }

        // Update camera to follow vehicle
        std::array<float, 3> position = vehicle.getPosition();
        float rotation = vehicle.getRotation();
        sceneManager.updateCameraFollowTarget(position[0], position[1], position[2], rotation);
        sceneManager.updateMinimapCamera(position[0], position[2]);

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
