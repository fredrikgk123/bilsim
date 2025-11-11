#include "game.hpp"
#include "logger.hpp"
#include <iostream>

Game::Game(threepp::Canvas& canvas)
    : canvas_(canvas),
      audioEnabled_(true),
      clock_() {
}

void Game::initialize() {
    Logger::info("Initializing game...");

    initializeScene();
    initializeVehicle();
    initializeObstacles();
    initializePowerups();
    initializeInput();
    initializeAudio();
    initializeUI();

    Logger::info("Game initialization complete.");
    std::cout << "Game initialization complete." << std::endl;
}

void Game::initializeScene() {
    sceneManager_ = std::make_unique<SceneManager>();

    auto size = canvas_.size();
    float aspectRatio = static_cast<float>(size.width()) / static_cast<float>(size.height());

    sceneManager_->setupCamera(aspectRatio);
    sceneManager_->setupRenderer(size);
    sceneManager_->setupLighting();
    sceneManager_->setupGround();
    sceneManager_->setupMinimapCamera(aspectRatio);
}

void Game::initializeVehicle() {
    // Create vehicle at origin
    vehicle_ = std::make_unique<Vehicle>();

    // Create vehicle renderer
    vehicleRenderer_ = std::make_unique<VehicleRenderer>(sceneManager_->getScene(), *vehicle_);

    // Load vehicle model
    if (!vehicleRenderer_->loadModel("assets/body.obj")) {
        Logger::warning("Failed to load vehicle model, using fallback");
        std::cerr << "Warning: Failed to load vehicle model, using fallback" << std::endl;
    }
}

void Game::initializeObstacles() {
    // Create obstacle manager with play area size and tree count
    obstacleManager_ = std::make_unique<ObstacleManager>(100.0f, 5);

    // Get obstacles from manager
    const auto& obstacles = obstacleManager_->getObstacles();

    // Create renderers for all obstacles
    for (const auto& obstacle : obstacles) {
        auto renderer = std::make_unique<ObstacleRenderer>(sceneManager_->getScene(), *obstacle);
        obstacleRenderers_.push_back(std::move(renderer));
    }
}

void Game::initializePowerups() {
    // Create powerup manager with count and play area size
    powerupManager_ = std::make_unique<PowerupManager>(3, 100.0f);

    // Get powerups from manager
    const auto& powerups = powerupManager_->getPowerups();

    // Create renderers for all powerups
    for (const auto& powerup : powerups) {
        auto renderer = std::make_unique<PowerupRenderer>(sceneManager_->getScene(), *powerup);
        powerupRenderers_.push_back(std::move(renderer));
    }
}

void Game::initializeInput() {
    inputHandler_ = std::make_unique<InputHandler>(*vehicle_, *sceneManager_);

    // Register input handler with canvas
    canvas_.addKeyListener(*inputHandler_);

    // Set reset callback
    inputHandler_->setResetCallback([this]() {
        // Reset vehicle position
        vehicle_->reset();

        // Reset all powerups
        if (powerupManager_) {
            powerupManager_->reset();
        }
    });
}

void Game::initializeAudio() {
    audioManager_ = std::make_unique<AudioManager>();

    if (!audioManager_->initialize("assets/carnoise.wav")) {
        Logger::warning("Failed to initialize audio system");
        std::cerr << "Warning: Failed to initialize audio system" << std::endl;
        audioEnabled_ = false;
    }
}

void Game::initializeUI() {
    imguiLayer_ = std::make_unique<ImGuiLayer>();
}

void Game::update(float deltaTime) {
    updateGameState(deltaTime);
    updateCamera();
    updateAudio();
}

void Game::updateGameState(float deltaTime) {
    // Update input
    if (inputHandler_) {
        inputHandler_->update(deltaTime);
    }

    // Update vehicle physics
    if (vehicle_) {
        vehicle_->update(deltaTime);
    }

    // Update vehicle renderer
    if (vehicleRenderer_ && vehicle_) {
        vehicleRenderer_->update(inputHandler_ ? inputHandler_->isLeftPressed() : false,
                                inputHandler_ ? inputHandler_->isRightPressed() : false);
    }

    // Check for powerup collisions
    if (powerupManager_ && vehicle_) {
        powerupManager_->handleCollisions(*vehicle_);
    }

    // Update powerup renderers
    for (auto& renderer : powerupRenderers_) {
        if (renderer) {
            renderer->update();
        }
    }
}

void Game::updateCamera() {
    if (!sceneManager_ || !vehicle_) {
        return;
    }

    auto pos = vehicle_->getPosition();
    float rotation = vehicle_->getRotation();
    float scale = vehicle_->getScale();
    bool nitrousActive = vehicle_->isNitrousActive();
    float velocity = vehicle_->getVelocity();
    float driftAngle = vehicle_->getDriftAngle();

    sceneManager_->updateCameraFollowTarget(pos[0], pos[1], pos[2], rotation, scale,
                                           nitrousActive, velocity, driftAngle);
    sceneManager_->updateMinimapCamera(pos[0], pos[2], scale);
    sceneManager_->updateCameraFOV(nitrousActive, velocity);
}

void Game::updateAudio() {
    if (audioEnabled_ && audioManager_ && vehicle_) {
        audioManager_->update(*vehicle_);
    }
}

void Game::render() {
    renderMainView();
    renderMinimap();
    renderUI();
}

void Game::renderMainView() {
    if (sceneManager_) {
        sceneManager_->render();
    }
}

void Game::renderMinimap() {
    if (sceneManager_) {
        sceneManager_->renderMinimap();
    }
}

void Game::renderUI() {
    if (imguiLayer_ && vehicle_) {
        auto size = canvas_.size();
        imguiLayer_->render(*vehicle_, size);
    }
}

