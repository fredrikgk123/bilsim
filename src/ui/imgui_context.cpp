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

