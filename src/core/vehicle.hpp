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
    void reset();

    // Getters
    std::vector<float> getPosition() const;
    float getRotation() const;
    std::vector<float> getSize() const;
    float getVelocity() const;

private:
    // Calculate turn rate based on current speed
    float calculateTurnRate() const;

    std::vector<float> position_;           // x, y, z coordinates
    std::vector<float> initialPosition_;    // For reset functionality
    float rotation_;                         // In radians
    float velocity_;                         // Current speed
    float acceleration_;                     // Current acceleration
    float maxSpeed_;                         // Maximum forward speed
    float turnSpeed_;                        // Turn rate
    float forwardAcceleration_;             // Forward acceleration rate
    float backwardAcceleration_;            // Backward acceleration rate
    std::vector<float> size_;               // Width, height, length
};
