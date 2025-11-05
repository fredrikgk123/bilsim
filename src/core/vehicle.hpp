#pragma once

#include <array>
#include <cmath>
#include <functional>
#include "game_object.hpp"

class Vehicle : public GameObject {
public:
    // Constructor - takes x, y, z starting position
    Vehicle(float x = 0.0f, float y = 0.0f, float z = 0.0f);

    // Control methods
    void accelerateForward() noexcept;
    void accelerateForward(float multiplier) noexcept; // New overload: apply a multiplier to forward acceleration (1.0 = default)
    void accelerateBackward() noexcept;
    void turn(float amount) noexcept;

    // Drift methods
    void startDrift() noexcept;
    void stopDrift() noexcept;
    [[nodiscard]] bool isDrifting() const noexcept;

    // Nitrous methods
    void activateNitrous() noexcept;
    void pickupNitrous() noexcept;
    [[nodiscard]] bool hasNitrous() const noexcept;
    [[nodiscard]] bool isNitrousActive() const noexcept;
    [[nodiscard]] float getNitrousTimeRemaining() const noexcept;

    // Override from GameObject
    void update(float deltaTime) override;
    void reset() override;

    // Getters
    [[nodiscard]] float getVelocity() const noexcept;
    [[nodiscard]] float getMaxSpeed() const noexcept;
    [[nodiscard]] float getDriftAngle() const noexcept;  // Get current drift angle for camera
    [[nodiscard]] int getCurrentGear() const noexcept;   // Get current gear number
    [[nodiscard]] float getRPM() const noexcept;         // Get current engine RPM (for UI/audio)

    // Setters for collision response
    void setVelocity(float velocity) noexcept;

    // Runtime scale for vehicle size (used by renderer/collisions)
    void setScale(float scale) noexcept;
    [[nodiscard]] float getScale() const noexcept;

    // Acceleration tuning (UI can call these)
    void setAccelerationMultiplier(float m) noexcept { accel_multiplier_ = m; }
    [[nodiscard]] float getAccelerationMultiplier() const noexcept { return accel_multiplier_; }

    // Callback for resetting camera to orbit
    void setResetCameraCallback(std::function<void()> &&callback) noexcept;

private:
    // Calculate turn rate based on current speed
    [[nodiscard]] float calculateTurnRate() const noexcept;

    // Gear system methods
    void updateGearShifting() noexcept;
    [[nodiscard]] float getGearAccelerationMultiplier() const noexcept;

    float velocity_;                          // Current speed
    float acceleration_;                      // Current acceleration

    // Drift state
    bool isDrifting_;                         // Whether car is in drift mode
    float driftAngle_;                        // Angle between facing direction and velocity direction

    // Nitrous state
    bool hasNitrous_;                         // Whether player has a nitrous pickup
    bool nitrousActive_;                      // Whether nitrous is currently active
    float nitrousTimeRemaining_;              // Time left for nitrous boost

    // Gear system state
    int currentGear_;                         // Current gear (0 = reverse, 1-5 = forward gears)
    float rpm_;                               // Current engine RPM

    // Runtime scale
    float scale_ = 1.0f;

    // External tuning controlled by UI (acceleration multiplier)
    float accel_multiplier_ = 1.0f;

    // Callback
    std::function<void()> resetCameraCallback_;
};
