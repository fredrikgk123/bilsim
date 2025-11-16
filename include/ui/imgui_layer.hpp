#pragma once

#include <threepp/threepp.hpp>
#include "core/interfaces/IVehicleState.hpp"

class ImGuiLayer {
public:
    ImGuiLayer();

    // Render ImGui widgets. Called between ImGui::NewFrame() and ImGui::Render().
    void render(const IVehicleState& vehicle, const threepp::WindowSize& size);

private:
    // Smoothed display state for needles to avoid jitter
    float displayedSpeedRatio_ = 0.0f;
    float displayedRpmRatio_ = 0.0f;
    float smoothingAlpha_ = 0.18f; // 0..1 (higher = snappier)
};
