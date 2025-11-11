#include "imgui_context.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

const char* ImGuiContext::getGLSLVersion() {
#ifdef __APPLE__
    return "#version 150";
#else
    return "#version 330 core";
#endif
}

bool ImGuiContext::initialize(void* windowPtr) {
    // Create ImGui context if it doesn't exist
    if (!ImGui::GetCurrentContext()) {
        ImGui::CreateContext();
    }

    // Initialize platform/renderer backends
    auto* window = static_cast<GLFWwindow*>(windowPtr);
    if (!window) {
        return false;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(getGLSLVersion());

    return true;
}

void ImGuiContext::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiContext::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiContext::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#include "game.hpp"
#include "game_config.hpp"

using namespace threepp;

Game::Game(Canvas& canvas)
    : canvas_(canvas),
      audioEnabled_(false) {
}

void Game::initialize() {
    initializeScene();
    initializeVehicle();
    initializeObstacles();
    initializePowerups();
    initializeInput();
    initializeAudio();
    initializeUI();
}

void Game::initializeScene() {
    sceneManager_ = std::make_unique<SceneManager>();
    sceneManager_->setupCamera(canvas_.aspect());
    sceneManager_->setupMinimapCamera(GameConfig::UI::MINIMAP_ASPECT_RATIO);
    sceneManager_->setupRenderer(canvas_.size());
    sceneManager_->setupLighting();
    sceneManager_->setupGround();
}

void Game::initializeVehicle() {
    // Create vehicle
    vehicle_ = std::make_unique<Vehicle>(
        GameConfig::World::SPAWN_POINT_X,
        GameConfig::World::SPAWN_POINT_Y,
        GameConfig::World::SPAWN_POINT_Z
    );

    // Create renderer
    vehicleRenderer_ = std::make_unique<VehicleRenderer>(
        sceneManager_->getScene(),
        *vehicle_
    );

    // Load custom model
    vehicleRenderer_->loadModel(GameConfig::Assets::CAR_MODEL_PATH);
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

    // Create renderers for all obstacles
    for (const auto& obstacle : obstacleManager_->getObstacles()) {
        auto renderer = std::make_unique<ObstacleRenderer>(
            sceneManager_->getScene(),
            *obstacle
        );
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

    // Create renderers for all powerups
    for (const auto& powerup : powerupManager_->getPowerups()) {
        auto renderer = std::make_unique<PowerupRenderer>(
            sceneManager_->getScene(),
            *powerup
        );
        renderer->update(); // Set initial position
        powerupRenderers_.push_back(std::move(renderer));
    }
}

void Game::initializeInput() {
    inputHandler_ = std::make_unique<InputHandler>(*vehicle_, *sceneManager_);
    canvas_.addKeyListener(*inputHandler_);

    // Set up reset callback to respawn powerups
    inputHandler_->setResetCallback([this]() {
        powerupManager_->reset();
    });
}

void Game::initializeAudio() {
    audioManager_ = std::make_unique<AudioManager>();
    audioEnabled_ = audioManager_->initialize(GameConfig::Assets::ENGINE_SOUND_PATH);
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
    // Update input and vehicle
    inputHandler_->update(deltaTime);
    vehicle_->update(deltaTime);
    vehicleRenderer_->update(
        inputHandler_->isLeftPressed(),
        inputHandler_->isRightPressed()
    );

    // Handle collisions
    obstacleManager_->handleCollisions(*vehicle_);

    // Update powerups
    powerupManager_->update(deltaTime);
    powerupManager_->handleCollisions(*vehicle_);

    // Update powerup renderers
    for (auto& renderer : powerupRenderers_) {
        renderer->update();
    }
}

void Game::updateCamera() {
    const auto vehicle_position = vehicle_->getPosition();
    const float vehicle_rotation = vehicle_->getRotation();
    const float vehicle_velocity = vehicle_->getVelocity();
    const float vehicle_scale = vehicle_->getScale();
    const float drift_angle = vehicle_->getDriftAngle();

    sceneManager_->updateCameraFollowTarget(
        vehicle_position[0], vehicle_position[1], vehicle_position[2],
        vehicle_rotation, vehicle_scale, vehicle_->isNitrousActive(),
        vehicle_velocity, drift_angle
    );

    sceneManager_->updateMinimapCamera(
        vehicle_position[0], vehicle_position[2], vehicle_scale
    );

    sceneManager_->updateCameraFOV(
        vehicle_->isNitrousActive(),
        std::abs(vehicle_velocity)
    );
}

void Game::updateAudio() {
    if (audioEnabled_) {
        audioManager_->update(*vehicle_);
    }
}

void Game::render() {
    renderMainView();
    renderMinimap();
    renderUI();
}

void Game::renderMainView() {
    GLRenderer& gl_renderer = sceneManager_->getRenderer();
    const WindowSize& window_size = canvas_.size();

    // Set viewport for main view
    gl_renderer.setViewport(0, 0, window_size.width(), window_size.height());
    gl_renderer.setScissor(0, 0, window_size.width(), window_size.height());
    gl_renderer.setScissorTest(false);

    // Render scene
    sceneManager_->getRenderer().render(
        sceneManager_->getScene(),
        sceneManager_->getCamera()
    );
}

void Game::renderMinimap() {
    GLRenderer& gl_renderer = sceneManager_->getRenderer();
    const WindowSize& window_size = canvas_.size();

    // Calculate minimap position (bottom-left corner)
    const int minimap_x = GameConfig::UI::MINIMAP_PADDING;
    const int minimap_y = window_size.height() - GameConfig::UI::MINIMAP_SIZE - GameConfig::UI::MINIMAP_PADDING;

    // Set viewport for minimap
    gl_renderer.setViewport(minimap_x, minimap_y, GameConfig::UI::MINIMAP_SIZE, GameConfig::UI::MINIMAP_SIZE);
    gl_renderer.setScissor(minimap_x, minimap_y, GameConfig::UI::MINIMAP_SIZE, GameConfig::UI::MINIMAP_SIZE);
    gl_renderer.setScissorTest(true);

    // Render minimap
    sceneManager_->renderMinimap();

    // Reset scissor test
    gl_renderer.setScissorTest(false);
}

void Game::renderUI() {
    GLRenderer& gl_renderer = sceneManager_->getRenderer();
    const WindowSize& window_size = canvas_.size();

    // Reset viewport for UI overlay
    gl_renderer.setViewport(0, 0, window_size.width(), window_size.height());

    // Render ImGui
    imguiLayer_->render(*vehicle_, window_size);
}

