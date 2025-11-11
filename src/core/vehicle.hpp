#pragma once

#include <functional>
#include "game_object.hpp"
#include "interfaces/IVehicleState.hpp"
#include "interfaces/IControllable.hpp"
#include <algorithm>

class Vehicle : public GameObject, public IVehicleState, public IControllable {
public:
    // Constructor - takes x, y, z starting position
    explicit Vehicle(float x = 0.0f, float y = 0.0f, float z = 0.0f);

    // Control methods (implement IControllable)
    void accelerateForward() noexcept override;
    void accelerateForward(float multiplier) noexcept; // New overload: apply a multiplier to forward acceleration (1.0 = default)
    void accelerateBackward() noexcept override;
    void turn(float amount) noexcept override;

    // Drift methods (implement IControllable)
    void startDrift() noexcept override;
    void stopDrift() noexcept override;
    [[nodiscard]] bool isDrifting() const noexcept override;

    // Nitrous methods (implement IControllable)
    void activateNitrous() noexcept override;
    void pickupNitrous() noexcept;
    [[nodiscard]] bool hasNitrous() const noexcept override;
    [[nodiscard]] bool isNitrousActive() const noexcept override;
    [[nodiscard]] float getNitrousTimeRemaining() const noexcept override;

    // Override from GameObject
    void update(float deltaTime) override;
    void reset() override;

    // Getters (implement IVehicleState)
    [[nodiscard]] float getVelocity() const noexcept override;
    [[nodiscard]] static float getMaxSpeed() noexcept;
    [[nodiscard]] float getDriftAngle() const noexcept override;  // Get current drift angle for camera
    [[nodiscard]] int getCurrentGear() const noexcept override;   // Get current gear number
    [[nodiscard]] float getRPM() const noexcept override;         // Get current engine RPM (for UI/audio)
    [[nodiscard]] float getSteeringInput() const noexcept override; // Get current steering input (-1 to 1)

    // Setters for collision response
    void setVelocity(float velocity) noexcept;

    // Runtime scale for vehicle size (used by renderer/collisions)
    void setScale(float scale) noexcept;
    [[nodiscard]] float getScale() const noexcept override;

    // Acceleration tuning (UI can call these)
    void setAccelerationMultiplier(float m) noexcept {
        // Clamp to reasonable range (0.1x to 5x)
        accelMultiplier_ = std::clamp(m, 0.1f, 5.0f);
    }
    [[nodiscard]] float getAccelerationMultiplier() const noexcept { return accelMultiplier_; }

    // Callback for resetting camera to orbit
    void setResetCameraCallback(std::function<void()> &&callback) noexcept;

private:
    // Calculate turn rate based on current speed
    [[nodiscard]] float calculateTurnRate() const noexcept;

    // Gear system methods
    void updateGearShifting() noexcept;
    [[nodiscard]] float getGearAccelerationMultiplier() const noexcept;

    // Update helper methods (extract from monolithic update())
    void updateNitrous(float deltaTime) noexcept;
    void updateVelocity(float deltaTime) noexcept;
    void updateRPM() noexcept;
    void updateDrift(float deltaTime) noexcept;
    void updatePosition(float deltaTime) noexcept;
    void decayAcceleration() noexcept;

    float velocity_;                          // Current speed
    float acceleration_;                      // Current acceleration
    float steeringInput_;                     // Current steering input (-1 to 1)

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
    float accelMultiplier_ = 1.0f;

    // Callback
    std::function<void()> resetCameraCallback_;
};
