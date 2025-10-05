#include "vehicle.hpp"
#include <cmath>

Vehicle::Vehicle(float x, float y, float z)
    : position_({x, y, z}),
      rotation_(0.0f),
      velocity_(0.0f),
      acceleration_(0.0f),
      maxSpeed_(25.0f),              // Maximum speed in units/second - tuned for responsive but controllable gameplay
      turnSpeed_(2.0f),               // Radians per second - allows ~115° turn per second for arcade-style handling
      forwardAcceleration_(12.0f),   // Units/s² - quick acceleration for responsive feel
      backwardAcceleration_(-6.0f),  // Half of forward - vehicles naturally reverse slower than they accelerate forward
      size_({1.0f, 0.5f, 2.0f}) {    // Default size: width, height, length
}

void Vehicle::accelerateForward() {
    acceleration_ = forwardAcceleration_;
}

void Vehicle::accelerateBackward() {
    acceleration_ = backwardAcceleration_;
}

void Vehicle::turn(float amount) {
    // Calculate speed-dependent turn rate
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
    // Get absolute velocity
    float absVelocity = std::abs(velocity_);

    // If nearly stopped, don't turn at all (prevents unrealistic spinning in place)
    const float minTurnSpeed = 0.1f;  // Minimum speed needed to turn - simulates real vehicle physics
    if (absVelocity < minTurnSpeed) {
        return 0.0f;
    }

    // Calculate turn rate as a proportion of current speed to max speed
    // At low speeds: turns slowly (e.g., 0.2x normal turn rate)
    // At high speeds: turns at full rate (1.0x normal turn rate)
    float speedRatio = absVelocity / maxSpeed_;

    // Use a square root curve for smooth transition - provides good turning at medium speeds
    // while still requiring some speed to turn effectively (more realistic than linear)
    float turnRate = std::sqrt(speedRatio);

    // Clamp between 0 and 1
    if (turnRate > 1.0f) {
        turnRate = 1.0f;
    }

    return turnRate;
}

void Vehicle::update(float deltaTime) {
    // Update velocity based on acceleration
    velocity_ += acceleration_ * deltaTime;

    // Apply basic friction (0.994^60 ≈ 0.74 after 1 second at 60fps)
    // This creates natural deceleration when not accelerating
    velocity_ *= 0.994f;

    // Clamp velocity to maximum speeds
    if (velocity_ > maxSpeed_) {
        velocity_ = maxSpeed_;
    } else if (velocity_ < -maxSpeed_ / 2) {
        velocity_ = -maxSpeed_ / 2;  // Reverse speed is half of forward - realistic constraint
    }

    // Update position based on velocity and rotation
    // Uses standard 2D rotation math (sin for x-axis, cos for z-axis in 3D space)
    float dx = std::sin(rotation_) * velocity_ * deltaTime;
    float dz = std::cos(rotation_) * velocity_ * deltaTime;
    position_[0] += dx;
    position_[2] += dz;

    // Reset acceleration (input must be applied each frame)
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

bool Vehicle::checkCollision(const std::vector<float>& objectPosition, float objectRadius) const {
    // Simple sphere-based collision detection
    float dx = position_[0] - objectPosition[0];
    float dy = position_[1] - objectPosition[1];
    float dz = position_[2] - objectPosition[2];
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

    // Calculate bounding sphere radius from vehicle dimensions
    float boundingSphere = std::sqrt(size_[0] * size_[0] + size_[1] * size_[1] + size_[2] * size_[2]) / 2;
    return distance < (boundingSphere + objectRadius);
}
