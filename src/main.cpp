#include <threepp/threepp.hpp>
#include "core/game.hpp"
#include "ui/imgui_context.hpp"
#include <iostream>
#include <stdexcept>

using namespace threepp;

int main() {
    try {
        std::cout << "Starting Bilsimulator..." << std::endl;

        Canvas canvas("Bilsimulator");

        // Initialize ImGui
        std::cout << "Initializing ImGui..." << std::endl;
        if (!ImGuiContext::initialize(canvas.windowPtr())) {
            std::cerr << "Error: Failed to initialize ImGui" << std::endl;
            return 1;
        }

        // Create and initialize game
        std::cout << "Creating game instance..." << std::endl;
        Game game(canvas);
        game.initialize();

        std::cout << "Entering main game loop..." << std::endl;

        // Main game loop
        canvas.animate([&game] {
            float deltaTime = game.getClock().getDelta();

            // Update game state
            game.update(deltaTime);

            // Start ImGui frame
            ImGuiContext::newFrame();

            // Render everything
            game.render();

            // Finish ImGui rendering
            ImGuiContext::render();
        });

        // Cleanup
        std::cout << "Shutting down..." << std::endl;
        ImGuiContext::shutdown();

        std::cout << "Bilsimulator exited successfully." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        ImGuiContext::shutdown();
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception occurred" << std::endl;
        ImGuiContext::shutdown();
        return 1;
    }
}
