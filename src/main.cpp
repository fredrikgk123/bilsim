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
    // Try multiple paths to find the audio file
    AudioManager audioManager;
    bool audioEnabled = false;

    std::vector<std::string> possiblePaths = {
        "src/audio/carnoise.wav",           // When run from project root
        "../src/audio/carnoise.wav",        // When run from build directory
        "../../src/audio/carnoise.wav",     // When run from nested build dir
        "carnoise.wav"                      // When audio file is copied to executable location
    };

    for (const auto& path : possiblePaths) {
        if (std::filesystem::exists(path)) {
            std::cout << "Attempting to load audio from: " << path << std::endl;
            audioEnabled = audioManager.initialize(path);
            if (audioEnabled) {
                std::cout << "Audio successfully loaded!" << std::endl;
                break;
            }
        }
    }

    if (!audioEnabled) {
        std::cout << "No audio file found. Checked paths:" << std::endl;
        for (const auto& path : possiblePaths) {
            std::cout << "  - " << path << " (exists: "
                      << (std::filesystem::exists(path) ? "yes" : "no") << ")" << std::endl;
        }
        std::cout << "Continuing without audio..." << std::endl;
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

        // Update audio based on vehicle state
        if (audioEnabled) {
            audioManager.update(vehicle);
        }

        // Render the scene
        sceneManager.getRenderer().render(sceneManager.getScene(), sceneManager.getCamera());
    });

    return 0;
}
