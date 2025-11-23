// AI Assistance: GitHub Copilot was used for OBJ model loading
// and 3D transformation matrix calculations.

#pragma once

#include <threepp/threepp.hpp>

#include "graphics/game_object_renderer.hpp"
#include "core/interfaces/IVehicleState.hpp"
#include <string>

class VehicleRenderer : public GameObjectRenderer {
  public:
    VehicleRenderer(threepp::Scene& scene, const IVehicleState& vehicleState);

    // Load a 3D model from file (OBJ format)
    bool loadModel(const std::string& modelPath);

    // Unload any custom model and fall back to the box model
    void unloadModel();

    // Apply a runtime scale to the vehicle model (either custom or fallback)
    void applyScale(float scale);

    // Update visual representation (override to animate wheels)
    // leftPressed/rightPressed: raw keyboard input for steering wheel animation
    void update(bool leftPressed = false, bool rightPressed = false);

    // Get steering wheel position in vehicle-local coordinates
    [[nodiscard]] std::array<float, 3> getSteeringWheelPosition() const noexcept;
    [[nodiscard]] bool hasSteeringWheel() const noexcept;

  protected:
    // Override to create vehicle-specific model
    void createModel() override;

  private:
    const IVehicleState& vehicleState_;
    bool useCustomModel_;
    std::shared_ptr<threepp::Object3D> customModelGroup_;
    float modelScale_ = 1.0f; // runtime scale applied to loaded model
    float actualAppliedScale_ = 1.0f; // the final computed scale (auto-scale * modelScale_)

    // Steering wheel object
    std::shared_ptr<threepp::Object3D> steeringWheel_;
    std::shared_ptr<threepp::Group> steeringWheelPivot_;
    bool steeringWheelPivotOnCustom_ = false;

    // Wheel objects (front-left, front-right, rear-left, rear-right)
    std::shared_ptr<threepp::Object3D> wheelFL_;
    std::shared_ptr<threepp::Object3D> wheelFR_;
    std::shared_ptr<threepp::Object3D> wheelRL_;
    std::shared_ptr<threepp::Object3D> wheelRR_;

    // Pivot groups for each wheel so steering yaw and spin can be applied separately
    std::shared_ptr<threepp::Group> wheelFLPivot_;
    std::shared_ptr<threepp::Group> wheelFRPivot_;
    std::shared_ptr<threepp::Group> wheelRLPivot_;
    std::shared_ptr<threepp::Group> wheelRRPivot_;

    // Helper methods for wheel handling
    bool loadWheelModels(const std::string& wheelsDir);
    void unloadWheelModels();
    void applyWheelScaleAndPosition(float appliedScale);

    // Helper methods for steering wheel
    bool loadSteeringWheel(const std::string& steeringWheelPath);
    void unloadSteeringWheel();
    void applySteeringWheelScaleAndPosition(float appliedScale);

    // Runtime state for wheel animation
    std::array<float,3> prevPosition_ = {0.0f, 0.0f, 0.0f}; // previous vehicle position
    float wheelSpinFL_ = 0.0f; // accumulated spin (radians)
    float wheelSpinFR_ = 0.0f;
    float wheelSpinRL_ = 0.0f;
    float wheelSpinRR_ = 0.0f;

    // Steering wheel smoothing
    float currentSteeringWheelRotation_ = 0.0f; // Current smoothed steering wheel rotation

    // Spin axis detection (so we rotate around the correct local axis)
    enum class WheelSpinAxis { X, Y, Z };

    // Steering wheel geometry center and rotation axis
    std::array<float,3> steeringWheelCenter_ = {0.0f, 0.0f, 0.0f};
    WheelSpinAxis steeringWheelRotationAxis_ = WheelSpinAxis::X;

    // Per-wheel geometry center (local) used for recentering so wheels spin around their own center
    std::array<float,3> wheelCenterFL_ = {0.0f, 0.0f, 0.0f};
    std::array<float,3> wheelCenterFR_ = {0.0f, 0.0f, 0.0f};
    std::array<float,3> wheelCenterRL_ = {0.0f, 0.0f, 0.0f};
    std::array<float,3> wheelCenterRR_ = {0.0f, 0.0f, 0.0f};

    // Spin axis detection for each wheel
    WheelSpinAxis wheelSpinAxisFL_ = WheelSpinAxis::X;
    WheelSpinAxis wheelSpinAxisFR_ = WheelSpinAxis::X;
    WheelSpinAxis wheelSpinAxisRL_ = WheelSpinAxis::X;
    WheelSpinAxis wheelSpinAxisRR_ = WheelSpinAxis::X;

    // Per-wheel invert multipliers (1 or -1) to flip spin direction when model orientation differs
    float wheelInvertFL_ = 1.0f;
    float wheelInvertFR_ = 1.0f;
    float wheelInvertRL_ = 1.0f;
    float wheelInvertRR_ = 1.0f;

    // Track whether pivot was attached to customModelGroup_ (true) or objectGroup_ (false)
    bool wheelFLPivotOnCustom_ = false;
    bool wheelFRPivotOnCustom_ = false;
    bool wheelRLPivotOnCustom_ = false;
    bool wheelRRPivotOnCustom_ = false;
};
