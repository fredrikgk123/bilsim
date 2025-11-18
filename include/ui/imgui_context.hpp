#pragma once

#include <threepp/extras/imgui/ImguiContext.hpp>
#include <memory>

/**
 * Helper class for initializing and cleaning up ImGui using threepp's built-in support
 */
class ImGuiContext {
public:
    // Initialize ImGui with GLFW window
    static bool initialize(void* windowPtr);

    // Shutdown ImGui
    static void shutdown();

    // Start a new frame
    static void newFrame();

    // Render ImGui
    static void render();

private:
    ImGuiContext() = default;

    static std::unique_ptr<ImguiContext> instance_;
};

