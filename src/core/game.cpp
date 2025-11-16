#include "core/game.hpp"
#include "core/game_config.hpp"
#include "core/logger.hpp"
#include <iostream>

Game::Game(threepp::Canvas& canvas)
    : canvas_(canvas),
      audioEnabled_(true),
      clock_(),
      lastWindowWidth_(0),
      lastWindowHeight_(0) {
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
    // Create vehicle at spawn point
    vehicle_ = std::make_unique<Vehicle>(
        GameConfig::World::SPAWN_POINT_X,
        GameConfig::World::SPAWN_POINT_Y,
        GameConfig::World::SPAWN_POINT_Z
    );

    // Create vehicle renderer
    vehicleRenderer_ = std::make_unique<VehicleRenderer>(sceneManager_->getScene(), *vehicle_);

    // Load custom model
    vehicleRenderer_->loadModel(GameConfig::Assets::CAR_MODEL_PATH);

    // Apply scale to the vehicle renderer
    vehicleRenderer_->applyScale(vehicle_->getScale());

    // Set up reset callback
    vehicle_->setResetCameraCallback([this]() {
        sceneManager_->setCameraMode(CameraMode::FOLLOW);
    });
}

void Game::initializeObstacles() {
    // Create obstacle manager
    obstacleManager_ = std::make_unique<ObstacleManager>(
        GameConfig::World::PLAY_AREA_SIZE,
        GameConfig::Obstacle::DEFAULT_TREE_COUNT
    );

    // Get obstacles from manager
    const auto& obstacles = obstacleManager_->getObstacles();

    // Create renderers for all obstacles
    for (const auto& obstacle : obstacles) {
        auto renderer = std::make_unique<ObstacleRenderer>(sceneManager_->getScene(), *obstacle);
        renderer->update(); // Set initial position
        obstacleRenderers_.push_back(std::move(renderer));
    }
}

void Game::initializePowerups() {
    // Create powerup manager
    powerupManager_ = std::make_unique<PowerupManager>(
        GameConfig::Powerup::DEFAULT_COUNT,
        GameConfig::World::PLAY_AREA_SIZE
    );

    // Get powerups from manager
    const auto& powerups = powerupManager_->getPowerups();

    // Create renderers for all powerups
    for (const auto& powerup : powerups) {
        auto renderer = std::make_unique<PowerupRenderer>(sceneManager_->getScene(), *powerup);
        renderer->update(); // Set initial position
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
    // Check for window resize
    auto size = canvas_.size();
    if (size.width() != lastWindowWidth_ || size.height() != lastWindowHeight_) {
        lastWindowWidth_ = size.width();
        lastWindowHeight_ = size.height();
        if (sceneManager_) {
            sceneManager_->resize(size);
        }
    }

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

    // Handle collisions
    if (obstacleManager_ && vehicle_) {
        obstacleManager_->handleCollisions(*vehicle_);
    }

    // Update powerups (rotation animation, etc.)
    if (powerupManager_) {
        powerupManager_->update(deltaTime);
    }

    // Check for powerup collisions
    if (powerupManager_ && vehicle_) {
        powerupManager_->handleCollisions(*vehicle_);
    }

    // Update powerup renderers (powerups can move/rotate)
    for (auto& renderer : powerupRenderers_) {
        if (renderer) {
            renderer->update();
        }
    }

    // Note: Obstacle renderers are NOT updated here - they're static and only
    // need update() called once during initialization
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
    if (!sceneManager_) return;

    auto& renderer = sceneManager_->getRenderer();
    auto size = canvas_.size();

    // Set viewport for main view (full window)
    renderer.setViewport(0, 0, size.width(), size.height());
    renderer.setScissor(0, 0, size.width(), size.height());
    renderer.setScissorTest(false);

    // Render main scene with main camera
    sceneManager_->render();
}

void Game::renderMinimap() {
    if (!sceneManager_) return;

    auto& renderer = sceneManager_->getRenderer();
    auto size = canvas_.size();

    // Fixed minimap size
    const int minimapSize = GameConfig::UI::MINIMAP_SIZE;
    const int minimapX = GameConfig::UI::MINIMAP_PADDING;
    const int minimapY = size.height() - minimapSize - GameConfig::UI::MINIMAP_PADDING;

    renderer.setViewport(minimapX, minimapY, minimapSize, minimapSize);
    renderer.setScissor(minimapX, minimapY, minimapSize, minimapSize);
    renderer.setScissorTest(true);

    sceneManager_->renderMinimap();

    renderer.setScissorTest(false);
}

void Game::renderUI() {
    if (!imguiLayer_ || !vehicle_) return;

    auto& renderer = sceneManager_->getRenderer();
    auto size = canvas_.size();

    // Reset viewport for UI overlay (full window)
    renderer.setViewport(0, 0, size.width(), size.height());

    // Render ImGui overlay
    imguiLayer_->render(*vehicle_, size);
}

