#pragma once

#include <array>
#include <cmath>
#include "gameObject.hpp"

class Vehicle : public GameObject {
public:
    // Constructor - takes x, y, z starting position
    Vehicle(float x = 0.0f, float y = 0.0f, float z = 0.0f);

    // Control methods
    void accelerateForward();
    void accelerateBackward();
    void turn(float amount);

    // Nitrous methods
    void activateNitrous();
    void pickupNitrous();
    bool hasNitrous() const;
    bool isNitrousActive() const;
    float getNitrousTimeRemaining() const;

    // Override from GameObject
    void update(float deltaTime) override;
    void reset() override;

    // Getters
    float getVelocity() const;
    float getMaxSpeed() const;

private:
    // Calculate turn rate based on current speed
    float calculateTurnRate() const;

    float velocity_;                          // Current speed
    float acceleration_;                      // Current acceleration

    // Nitrous state
    bool hasNitrous_;                         // Whether player has a nitrous pickup
    bool nitrousActive_;                      // Whether nitrous is currently active
    float nitrousTimeRemaining_;              // Time left for nitrous boost
};
