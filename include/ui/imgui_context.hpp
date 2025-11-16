#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

struct GLFWwindow;

/**
 * Helper class for initializing and cleaning up ImGui
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

    static const char* getGLSLVersion();
};

