#pragma once

#include <threepp/extras/imgui/ImguiContext.hpp>
#include <memory>
#include <stdexcept>

/**
 * RAII wrapper for ImGui context using threepp's built-in support.
 * Ensures proper initialization and cleanup with exception safety.
 * Uses smart pointers exclusively - no raw pointer management.
 */
class ImGuiContext {
public:
    // RAII initialization - throws on failure
    explicit ImGuiContext(void* windowPtr);

    // Destructor handles cleanup automatically
    ~ImGuiContext();

    // Delete copy and move operations - resource should not be duplicated
    ImGuiContext(const ImGuiContext&) = delete;
    ImGuiContext& operator=(const ImGuiContext&) = delete;
    ImGuiContext(ImGuiContext&&) = delete;
    ImGuiContext& operator=(ImGuiContext&&) = delete;

    // Start a new frame
    void newFrame();

    // Render ImGui
    void render();

    // Check if initialized successfully
    [[nodiscard]] bool isInitialized() const noexcept { return initialized_; }

private:
    std::unique_ptr<ImguiContext> instance_;
    bool initialized_;
};

