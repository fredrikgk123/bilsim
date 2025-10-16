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

namespace {
    // Game configuration constants
    constexpr int POWERUP_COUNT = 20;
    constexpr float PLAY_AREA_SIZE = 200.0f;
    constexpr float SPAWN_X = 0.0f;
    constexpr float SPAWN_Y = 0.0f;
    constexpr float SPAWN_Z = 0.0f;

    // UI layout constants
    constexpr int MINIMAP_SIZE = 150;
    constexpr int MINIMAP_PADDING = 10;
    constexpr float MINIMAP_ASPECT_RATIO = 1.0f;

    // Asset paths
    const std::string CAR_MODEL_PATH = "assets/Chevrolet_Camaro_SS_High.obj";
    const std::string ENGINE_SOUND_PATH = "assets/carnoise.wav";
}

int main() {
    Canvas canvas("Bilsimulator");

    // Initialize scene
    SceneManager sceneManager;
    sceneManager.setupCamera(canvas.aspect());
    sceneManager.setupMinimapCamera(MINIMAP_ASPECT_RATIO);
    sceneManager.setupRenderer(canvas.size());
    sceneManager.setupLighting();
    sceneManager.setupGround();

    // Create vehicle and renderer
    Vehicle vehicle(SPAWN_X, SPAWN_Y, SPAWN_Z);
    VehicleRenderer vehicleRenderer(sceneManager.getScene(), vehicle);

    // Load custom car model
    vehicleRenderer.loadModel(CAR_MODEL_PATH);

    // Create powerup manager with randomly placed powerups
    PowerupManager powerupManager(POWERUP_COUNT, PLAY_AREA_SIZE);

    // Create renderers for all powerups
    std::vector<std::unique_ptr<PowerupRenderer>> powerupRenderers;
    for (const auto& powerup : powerupManager.getPowerups()) {
        auto renderer = std::make_unique<PowerupRenderer>(sceneManager.getScene(), *powerup);
        powerupRenderers.push_back(std::move(renderer));
    }

    // Setup input handling
    std::unique_ptr<InputHandler> inputHandler = std::make_unique<InputHandler>(vehicle, sceneManager);
    canvas.addKeyListener(*inputHandler);

    // Set up reset callback to respawn powerups
    inputHandler->setResetCallback([&powerupManager]() {
        powerupManager.reset();
    });

    // Set up vehicle reset callback to reset camera to follow mode
    vehicle.setResetCameraCallback([&sceneManager]() {
        sceneManager.setCameraMode(CameraMode::FOLLOW);
    });

    // Initialize audio
    AudioManager audioManager;
    bool audioEnabled = audioManager.initialize(ENGINE_SOUND_PATH);

    if (!audioEnabled) {
        std::cout << "Audio file '" << ENGINE_SOUND_PATH << "' not found. Continuing without audio..." << std::endl;
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
        for (auto& powerupRenderer : powerupRenderers) {
            powerupRenderer->update();
        }

        // Update camera to follow vehicle
        const std::array<float, 3> vehiclePosition = vehicle.getPosition();
        const float vehicleRotation = vehicle.getRotation();
        sceneManager.updateCameraFollowTarget(vehiclePosition[0], vehiclePosition[1], vehiclePosition[2],
                                              vehicleRotation, vehicle.isNitrousActive());
        sceneManager.updateMinimapCamera(vehiclePosition[0], vehiclePosition[2]);

        // Update camera FOV based on nitrous state (speed FOV effect)
        sceneManager.updateCameraFOV(vehicle.isNitrousActive());

        // Update audio
        if (audioEnabled) {
            audioManager.update(vehicle);
        }

        GLRenderer& glRenderer = sceneManager.getRenderer();
        const WindowSize& windowSize = canvas.size();

        // Render main view
        glRenderer.setViewport(0, 0, windowSize.width(), windowSize.height());
        glRenderer.setScissor(0, 0, windowSize.width(), windowSize.height());
        glRenderer.setScissorTest(false);
        sceneManager.getRenderer().render(sceneManager.getScene(), sceneManager.getCamera());

        // Render minimap in bottom-left corner
        const int minimapX = MINIMAP_PADDING;
        const int minimapY = windowSize.height() - MINIMAP_SIZE - MINIMAP_PADDING;
        glRenderer.setViewport(minimapX, minimapY, MINIMAP_SIZE, MINIMAP_SIZE);
        glRenderer.setScissor(minimapX, minimapY, MINIMAP_SIZE, MINIMAP_SIZE);
        glRenderer.setScissorTest(true);
        sceneManager.renderMinimap();
        glRenderer.setScissorTest(false);

        // Render UI overlay
        glRenderer.setViewport(0, 0, windowSize.width(), windowSize.height());
        uiManager.render(vehicle, windowSize);
    });

    return 0;
}
