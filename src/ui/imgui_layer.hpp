#pragma once

#include <threepp/threepp.hpp>
#include "../core/vehicle.hpp"

class ImGuiLayer {
public:
    ImGuiLayer();

    // Render ImGui widgets. Called between ImGui::NewFrame() and ImGui::Render().
    void render(const Vehicle& vehicle, const threepp::WindowSize& size);

    // Poll/consume requests from the UI
    // When this returns true, out_scale will contain the requested scale for the new vehicle
    bool consumeRespawnRequest(float& out_scale) noexcept;
    // When this returns true, the caller should temporarily remove any loaded .obj model
    bool consumeScrapMeshRequest() noexcept;
    // When this returns true, the caller should reload a previously set model
    bool consumeReloadModelRequest() noexcept;

    // Returns the current acceleration multiplier (1.0 = default)
    [[nodiscard]] float getAccelerationMultiplier() const noexcept { return accel_multiplier_; }

private:
    bool show_test_window_ = true;
    int counter_ = 0;

    // Smoothed display state for needles to avoid jitter
    float displayed_speed_ratio_ = 0.0f;
    float displayed_rpm_ratio_ = 0.0f;
    float smoothing_alpha_ = 0.18f; // 0..1 (higher = snappier)

    // Developer/test controls for spawning/scale
    float car_scale_ = 1.0f; // 0.5 .. 3.0
    bool respawn_requested_ = false;
    bool scrap_mesh_requested_ = false;
    bool reload_model_requested_ = false;

    // multiplier applied to vehicle acceleration (UI controlled)
    // range: 0.1 .. 3.0 (adjustable via slider)
    float accel_multiplier_ = 1.0f;
};
