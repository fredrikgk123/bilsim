#include <threepp/threepp.hpp>
#include <memory>
#include <iostream>
#include <vector>
#include "core/vehicle.hpp"
#include "core/powerup_manager.hpp"
#include "core/obstacle_manager.hpp"
#include "graphics/vehicle_renderer.hpp"
#include "graphics/powerup_renderer.hpp"
#include "graphics/obstacle_renderer.hpp"
#include "graphics/scene_manager.hpp"
#include "input/input_handler.hpp"
#include "audio/audio_manager.hpp"
#include "ui/imgui_layer.hpp"

// ImGui and backend headers
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace threepp;

namespace {
    // Game configuration constants
    constexpr int POWERUP_COUNT = 20;
    constexpr int TREE_COUNT = 30;
    constexpr float PLAY_AREA_SIZE = 200.0f;
    constexpr float SPAWN_X = 0.0f;
    constexpr float SPAWN_Y = 0.0f;
    constexpr float SPAWN_Z = 0.0f;

    // UI layout constants
    constexpr int MINIMAP_SIZE = 150;
    constexpr int MINIMAP_PADDING = 10;
    constexpr float MINIMAP_ASPECT_RATIO = 1.0f;

    // Asset paths
    const std::string CAR_MODEL_PATH = "assets/body.obj";
    const std::string ENGINE_SOUND_PATH = "assets/carnoise.wav";
}

int main() {
    Canvas canvas("Bilsimulator");

    // Initialize ImGui backends using the native GLFW window pointer from threepp::Canvas
    // We'll choose GLSL version depending on platform (macOS requires `#version 150`).
#ifdef __APPLE__
    const char *glsl_version = "#version 150";
#else
    const char *glsl_version = "#version 330 core";
#endif

    // Create ImGui context early
    if (!ImGui::GetCurrentContext()) {
        ImGui::CreateContext();
    }

    // Initialize platform/renderer backends
    void *window_ptr = canvas.windowPtr();
    GLFWwindow *window = static_cast<GLFWwindow *>(window_ptr);
    if (window) {
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    // Initialize scene
    SceneManager scene_manager;
    scene_manager.setupCamera(canvas.aspect());
    scene_manager.setupMinimapCamera(MINIMAP_ASPECT_RATIO);
    scene_manager.setupRenderer(canvas.size());
    scene_manager.setupLighting();
    scene_manager.setupGround();

    // Create vehicle and renderer
    Vehicle vehicle(SPAWN_X, SPAWN_Y, SPAWN_Z);
    VehicleRenderer vehicle_renderer(scene_manager.getScene(), vehicle);

    // Load custom car model
    vehicle_renderer.loadModel(CAR_MODEL_PATH);
    // Ensure initial model matches vehicle scale
    vehicle_renderer.applyScale(vehicle.getScale());

    // Create obstacle manager with walls and trees
    ObstacleManager obstacle_manager(PLAY_AREA_SIZE, TREE_COUNT);

    // Create renderers for all obstacles
    std::vector<std::unique_ptr<ObstacleRenderer> > obstacle_renderers;
    for (const auto &obstacle: obstacle_manager.getObstacles()) {
        auto renderer = std::make_unique<ObstacleRenderer>(scene_manager.getScene(), *obstacle);
        obstacle_renderers.push_back(std::move(renderer));
    }

    // Create powerup manager with randomly placed powerups
    PowerupManager powerup_manager(POWERUP_COUNT, PLAY_AREA_SIZE);

    // Create renderers for all powerups
    std::vector<std::unique_ptr<PowerupRenderer> > powerup_renderers;
    for (const auto &powerup: powerup_manager.getPowerups()) {
        auto renderer = std::make_unique<PowerupRenderer>(scene_manager.getScene(), *powerup);
        powerup_renderers.push_back(std::move(renderer));
    }

    // Setup input handling
    std::unique_ptr<InputHandler> input_handler = std::make_unique<InputHandler>(vehicle, scene_manager);
    canvas.addKeyListener(*input_handler);

    // Set up reset callback to respawn powerups
    input_handler->setResetCallback([&powerup_manager]() {
        powerup_manager.reset();
    });

    // Set up vehicle reset callback to reset camera to follow mode
    vehicle.setResetCameraCallback([&scene_manager]() {
        scene_manager.setCameraMode(CameraMode::FOLLOW);
    });

    // Initialize audio
    AudioManager audio_manager;
    bool audio_enabled = audio_manager.initialize(ENGINE_SOUND_PATH);


    // Setup UI (use ImGui-based HUD instead of threepp HUD)
    ImGuiLayer imgui_layer;

    // Main game loop
    Clock clock;

    canvas.animate([&] {
        float delta_time = clock.getDelta();

        // Update game state

        input_handler->update(delta_time);
        vehicle.update(delta_time);
        vehicle_renderer.update(input_handler->isLeftPressed(), input_handler->isRightPressed());

        // Handle obstacle collisions
        obstacle_manager.handleCollisions(vehicle);

        // Update powerups and handle collisions
        powerup_manager.update(delta_time);
        powerup_manager.handleCollisions(vehicle);

        // Update powerup renderers
        for (auto &powerup_renderer: powerup_renderers) {
            powerup_renderer->update();
        }

        // Update camera to follow vehicle
        const std::array<float, 3> vehicle_position = vehicle.getPosition();
        const float vehicle_rotation = vehicle.getRotation();
        const float vehicle_velocity = vehicle.getVelocity();
        const float vehicle_scale = vehicle.getScale();
        const float drift_angle = vehicle.getDriftAngle();

        scene_manager.updateCameraFollowTarget(vehicle_position[0], vehicle_position[1], vehicle_position[2],
                                               vehicle_rotation, vehicle_scale, vehicle.isNitrousActive(),
                                               vehicle_velocity, drift_angle);
        scene_manager.updateMinimapCamera(vehicle_position[0], vehicle_position[2], vehicle_scale);

        // Update camera FOV based on speed and nitrous state (speed FOV effect)
        scene_manager.updateCameraFOV(vehicle.isNitrousActive(), std::abs(vehicle_velocity));

        // Update audio
        if (audio_enabled) {
            audio_manager.update(vehicle);
        }

        GLRenderer &gl_renderer = scene_manager.getRenderer();
        const WindowSize &window_size = canvas.size();

        // Render main view
        gl_renderer.setViewport(0, 0, window_size.width(), window_size.height());
        gl_renderer.setScissor(0, 0, window_size.width(), window_size.height());
        gl_renderer.setScissorTest(false);
        scene_manager.getRenderer().render(scene_manager.getScene(), scene_manager.getCamera());

        // Render minimap in bottom-left corner
        const int minimap_x = MINIMAP_PADDING;
        const int minimap_y = window_size.height() - MINIMAP_SIZE - MINIMAP_PADDING;
        gl_renderer.setViewport(minimap_x, minimap_y, MINIMAP_SIZE, MINIMAP_SIZE);
        gl_renderer.setScissor(minimap_x, minimap_y, MINIMAP_SIZE, MINIMAP_SIZE);
        gl_renderer.setScissorTest(true);
        scene_manager.renderMinimap();
        gl_renderer.setScissorTest(false);

        // Render UI overlay
        gl_renderer.setViewport(0, 0, window_size.width(), window_size.height());

        // Start ImGui frame (platform + renderer backends)
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui widgets (HUD + developer window)
        imgui_layer.render(vehicle, window_size);


        // Render ImGui draw data via backend
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });

    // Shutdown ImGui backends and context
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
