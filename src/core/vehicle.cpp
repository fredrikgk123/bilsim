#include "vehicle.hpp"
#include <cmath>

Vehicle::Vehicle(float x, float y, float z)
    : position_({x, y, z}),
      initialPosition_({x, y, z}),
      rotation_(0.0f),
      velocity_(0.0f),
      acceleration_(0.0f),
      maxSpeed_(25.0f),                  // 25 units/sec - balanced for responsive but controllable gameplay
      turnSpeed_(2.0f),                  // 2 radians/sec (~115°/sec) - arcade-style handling
      forwardAcceleration_(12.0f),       // 12 units/sec² - reaches max speed in ~2 seconds
      backwardAcceleration_(-6.0f),      // Half of forward - vehicles reverse slower than they accelerate
      size_({1.0f, 0.5f, 2.0f}) {        // Width=1, Height=0.5, Length=2 - compact car proportions
}

void Vehicle::accelerateForward() {
    acceleration_ = forwardAcceleration_;
}

void Vehicle::accelerateBackward() {
    acceleration_ = backwardAcceleration_;
}

void Vehicle::turn(float amount) {
    float turnRate = calculateTurnRate();
    rotation_ += amount * turnSpeed_ * turnRate;

    // Normalize rotation to [0, 2π]
    while (rotation_ >= 2 * M_PI) {
        rotation_ -= 2 * M_PI;
    }
    while (rotation_ < 0) {
        rotation_ += 2 * M_PI;
    }
}

float Vehicle::calculateTurnRate() const {
    float absVelocity = std::abs(velocity_);
    const float minTurnSpeed = 0.1f;   // 0.1 units/sec minimum - prevents spinning in place (realistic)

    // Don't turn if nearly stopped
    if (absVelocity < minTurnSpeed) {
        return 0.0f;
    }

    // Turn rate scales with speed (using square root for smooth curve)
    float speedRatio = absVelocity / maxSpeed_;
    float turnRate = std::sqrt(speedRatio);  // Square root gives good control at medium speeds

    return turnRate > 1.0f ? 1.0f : turnRate;
}

void Vehicle::update(float deltaTime) {
    // Update velocity based on acceleration
    velocity_ += acceleration_ * deltaTime;

    // Apply friction
    velocity_ *= 0.994f;  // 0.994^60 ≈ 0.74 after 1 second at 60fps - natural deceleration

    // Clamp velocity to max speeds
    if (velocity_ > maxSpeed_) {
        velocity_ = maxSpeed_;
    } else if (velocity_ < -maxSpeed_ / 2) {  // Reverse is half speed - realistic constraint
        velocity_ = -maxSpeed_ / 2;
    }

    // Update position based on velocity and rotation
    float dx = std::sin(rotation_) * velocity_ * deltaTime;
    float dz = std::cos(rotation_) * velocity_ * deltaTime;
    position_[0] += dx;
    position_[2] += dz;

    // Reset acceleration (must be reapplied each frame)
    acceleration_ = 0.0f;
}

void Vehicle::reset() {
    position_ = initialPosition_;
    rotation_ = 0.0f;
    velocity_ = 0.0f;
    acceleration_ = 0.0f;
}

std::vector<float> Vehicle::getPosition() const {
    return position_;
}

float Vehicle::getRotation() const {
    return rotation_;
}

std::vector<float> Vehicle::getSize() const {
    return size_;
}

float Vehicle::getVelocity() const {
    return velocity_;
}
