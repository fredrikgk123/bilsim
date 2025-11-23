#include <threepp/threepp.hpp>
#include "core/game.hpp"
#include "ui/imgui_context.hpp"
#include <iostream>
#include <stdexcept>
#include <memory>

using namespace threepp;

int main() {
    try {
        std::cout << "Starting Bilsimulator..." << std::endl;

        // Create canvas
        Canvas canvas("Bilsimulator");

        // Initialize ImGui with RAII - automatic cleanup on scope exit
        std::cout << "Initializing ImGui..." << std::endl;
        auto imguiContext = std::make_unique<ImGuiContext>(canvas.windowPtr());

        if (!imguiContext->isInitialized()) {
            std::cerr << "Error: ImGui initialization failed" << std::endl;
            return 1;
        }

        // Create and initialize game
        std::cout << "Creating game instance..." << std::endl;
        auto game = std::make_unique<Game>(canvas);
        game->initialize();

        std::cout << "Entering main game loop..." << std::endl;

        // Main game loop with error handling
        canvas.animate([&game, &imguiContext] {
            try {
                // Check if we should exit
                if (game->shouldExit()) {
                    return;
                }

                float deltaTime = game->getClock().getDelta();

                // Update game state
                game->update(deltaTime);

                // Start ImGui frame
                imguiContext->newFrame();

                // Render everything
                game->render();

                // Finish ImGui rendering
                imguiContext->render();

            } catch (const std::exception& e) {
                std::cerr << "Error in game loop: " << e.what() << std::endl;
                game->requestExit();
            } catch (...) {
                std::cerr << "Unknown error in game loop" << std::endl;
                game->requestExit();
            }
        });

        // Cleanup happens automatically via RAII
        // smart pointers (game, imguiContext) destruct in reverse order
        std::cout << "Shutting down..." << std::endl;
        game.reset();  // Explicit cleanup of game first
        imguiContext.reset();  // Then ImGui

        std::cout << "Bilsimulator exited successfully." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error during initialization: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception during initialization" << std::endl;
        return 1;
    }
}
