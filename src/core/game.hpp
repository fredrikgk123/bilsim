#pragma once

#include <memory>
#include <vector>
#include <threepp/threepp.hpp>
#include "vehicle.hpp"
#include "powerup_manager.hpp"
#include "obstacle_manager.hpp"
#include "../graphics/vehicle_renderer.hpp"
#include "../graphics/powerup_renderer.hpp"
#include "../graphics/obstacle_renderer.hpp"
#include "../graphics/scene_manager.hpp"
#include "../input/input_handler.hpp"
#include "../audio/audio_manager.hpp"
#include "../ui/imgui_layer.hpp"

/**
 * Main game class - encapsulates all game logic and state
 * Manages initialization, game loop, and cleanup
 */
class Game {
public:
    explicit Game(threepp::Canvas& canvas);
    ~Game() = default;

    // Initialize all game systems
    void initialize();

    // Main game loop (called every frame)
    void update(float deltaTime);

    // Render the game
    void render();

    // Get the game clock
    threepp::Clock& getClock() { return clock_; }

private:
    // Initialization helpers
    void initializeScene();
    void initializeVehicle();
    void initializeObstacles();
    void initializePowerups();
    void initializeInput();
    void initializeAudio();
    void initializeUI();

    // Update helpers
    void updateGameState(float deltaTime);
    void updateCamera();
    void updateAudio();

    // Render helpers
    void renderMainView();
    void renderMinimap();
    void renderUI();

    // Canvas reference
    threepp::Canvas& canvas_;

    // Core systems
    std::unique_ptr<SceneManager> sceneManager_;
    std::unique_ptr<Vehicle> vehicle_;
    std::unique_ptr<VehicleRenderer> vehicleRenderer_;

    // Managers
    std::unique_ptr<ObstacleManager> obstacleManager_;
    std::unique_ptr<PowerupManager> powerupManager_;

    // Renderers
    std::vector<std::unique_ptr<ObstacleRenderer>> obstacleRenderers_;
    std::vector<std::unique_ptr<PowerupRenderer>> powerupRenderers_;

    // Input and UI
    std::unique_ptr<InputHandler> inputHandler_;
    std::unique_ptr<AudioManager> audioManager_;
    std::unique_ptr<ImGuiLayer> imguiLayer_;

    // State
    bool audioEnabled_;
    threepp::Clock clock_;

    // Window size tracking for resize detection
    int lastWindowWidth_;
    int lastWindowHeight_;
};

