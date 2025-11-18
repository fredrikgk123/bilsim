#include "ui/imgui_context.hpp"

std::unique_ptr<ImguiContext> ImGuiContext::instance_ = nullptr;

bool ImGuiContext::initialize(void* windowPtr) {
    if (!windowPtr) {
        return false;
    }

    try {
        // Create threepp's ImguiContext with an empty lambda
        // The actual UI content will be drawn by ImGuiLayer between newFrame() and render()
        instance_ = std::make_unique<ImguiFunctionalContext>(windowPtr, []() {});
        return true;
    } catch (...) {
        return false;
    }
}

void ImGuiContext::shutdown() {
    instance_.reset();
}

void ImGuiContext::newFrame() {
    // threepp's ImguiContext::render() calls these internally, but we need them
    // separately for our rendering flow
    if (!instance_) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiContext::render() {
    if (!instance_) return;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

