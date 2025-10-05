#pragma once

#include <vector>

class Vehicle {
public:
    Vehicle(float x = 0.0f, float y = 0.0f, float z = 0.0f);

    // Control methods
    void accelerateForward();
    void accelerateBackward();
    void turn(float amount);
    void update(float deltaTime);

    // Getters
    std::vector<float> getPosition() const;
    float getRotation() const;
    std::vector<float> getSize() const;
    float getVelocity() const;

    // Basic collision detection
    bool checkCollision(const std::vector<float>& objectPosition, float objectRadius) const;

private:
    // Helper function to calculate turn rate based on speed
    float calculateTurnRate() const;

    std::vector<float> position_;      // x, y, z
    float rotation_;                   // in radians
    float velocity_;                   // current speed
    float acceleration_;               // current acceleration
    float maxSpeed_;                   // maximum speed
    float turnSpeed_;                  // how fast the vehicle can turn
    float forwardAcceleration_;        // forward acceleration amount
    float backwardAcceleration_;       // backward acceleration amount
    std::vector<float> size_;          // width, height, length
};
