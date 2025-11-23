// AI Assistance: GitHub Copilot was used for RAII wrapper pattern
// and exception-safe resource management implementation.

#include "ui/imgui_context.hpp"
#include <iostream>
#include <stdexcept>

ImGuiContext::ImGuiContext(void* windowPtr)
    : instance_(nullptr), initialized_(false) {

    if (!windowPtr) {
        throw std::invalid_argument("ImGuiContext: windowPtr cannot be null");
    }

    try {
        // Create threepp's ImguiContext with an empty lambda
        // The actual UI content will be drawn by ImGuiLayer between newFrame() and render()
        instance_ = std::make_unique<ImguiFunctionalContext>(windowPtr, []() {});
        initialized_ = true;
    } catch (const std::exception& e) {
        std::cerr << "ImGuiContext initialization failed: " << e.what() << std::endl;
        throw std::runtime_error(std::string("Failed to initialize ImGui: ") + e.what());
    } catch (...) {
        std::cerr << "ImGuiContext initialization failed with unknown error" << std::endl;
        throw std::runtime_error("Failed to initialize ImGui: unknown error");
    }
}

ImGuiContext::~ImGuiContext() {
    // Smart pointer automatically cleans up resources
    // No manual cleanup needed - RAII handles everything
    instance_.reset();
    initialized_ = false;
}

void ImGuiContext::newFrame() {
    // threepp's ImguiContext::render() calls these internally, but we need them
    // separately for our rendering flow
    if (!initialized_ || !instance_) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiContext::render() {
    if (!initialized_ || !instance_) {
        return;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

