#include "vehicle.hpp"
#include <cmath>

// Anonymous namespace - these constants are LOCAL to this file only (NOT global!)
namespace {
    // Physics constants (realistic tuning)
    const float MAX_SPEED = 41.67f;                  // ~150 km/h (realistic for a small car)
    const float MAX_REVERSE_SPEED = 13.9f;           // ~50 km/h reverse
    const float TURN_SPEED = 1.0f;                  // 1 rad/sec (~57°/sec)
    const float FORWARD_ACCELERATION = 8.0f;        // Balanced acceleration
    const float BACKWARD_ACCELERATION = -4.0f;      // Slower reverse acceleration
    const float FRICTION_COEFFICIENT = 0.997f;      // More friction for tighter control
    const float MIN_TURN_SPEED = 0.5f;              // Prevents spinning at very low speed

    // Vehicle dimensions
    const float VEHICLE_WIDTH = 1.0f;
    const float VEHICLE_HEIGHT = 0.5f;
    const float VEHICLE_LENGTH = 2.0f;
}

Vehicle::Vehicle(float x, float y, float z)
    : GameObject(x, y, z),
      velocity_(0.0f),
      acceleration_(0.0f) {
    // Set vehicle-specific size
    size_[0] = VEHICLE_WIDTH;
    size_[1] = VEHICLE_HEIGHT;
    size_[2] = VEHICLE_LENGTH;
}

void Vehicle::accelerateForward() {
    acceleration_ = FORWARD_ACCELERATION;
}

void Vehicle::accelerateBackward() {
    acceleration_ = BACKWARD_ACCELERATION;
}

void Vehicle::turn(float amount) {
    float turnRate = calculateTurnRate();
    rotation_ = rotation_ + (amount * TURN_SPEED * turnRate);

    // Normalize rotation to [0, 2π]
    rotation_ = std::fmod(rotation_, 2.0f * static_cast<float>(M_PI));
    if (rotation_ < 0.0f) {
        rotation_ = rotation_ + (2.0f * static_cast<float>(M_PI));
    }
}

float Vehicle::calculateTurnRate() const {
    float absVelocity = std::abs(velocity_);

    // Don't turn if completely stopped
    if (absVelocity < 0.1f) {
        return 0.0f;
    }

    // Extremely low speeds (0.1-0.3 m/s / ~0.4-1.1 km/h): very minimal turning
    // Almost stopped - barely any turning ability
    if (absVelocity < 0.3f) {
        // Linear scaling from 0.05 at 0.1 m/s to 0.15 at 0.3 m/s
        float turnRate = 0.05f + ((absVelocity - 0.1f) / 0.2f) * 0.1f;
        return turnRate;
    }

    // Very low speeds (0.3-3 m/s / ~1.1-11 km/h): minimal but usable turning
    // This allows parking-speed maneuvers
    if (absVelocity < 3.0f) {
        // Linear scaling from 0.15 at 0.3 m/s to 0.5 at 3 m/s
        float turnRate = 0.15f + ((absVelocity - 0.3f) / 2.7f) * 0.35f;
        return turnRate;
    }

    // Low to medium speeds (3-15 m/s / ~11-54 km/h): good turning capability
    if (absVelocity < 15.0f) {
        // Linear scaling from 0.5 at 3 m/s to 1.0 at 15 m/s
        float turnRate = 0.5f + ((absVelocity - 3.0f) / 12.0f) * 0.5f;
        return turnRate;
    }

    // High speeds (15+ m/s / 54+ km/h): reduced turn rate for realism
    float speedRatio = (absVelocity - 15.0f) / (MAX_SPEED - 15.0f);
    float turnRate = 1.0f - (speedRatio * 0.4f);  // Reduces to 60% at max speed

    // Clamp to reasonable range
    if (turnRate < 0.6f) {
        turnRate = 0.6f;
    }
    if (turnRate > 1.0f) {
        turnRate = 1.0f;
    }

    return turnRate;
}

void Vehicle::update(float deltaTime) {
    // Update velocity based on acceleration
    velocity_ = velocity_ + (acceleration_ * deltaTime);

    // Apply friction
    velocity_ = velocity_ * FRICTION_COEFFICIENT;

    // Clamp velocity to max speeds
    if (velocity_ > MAX_SPEED) {
        velocity_ = MAX_SPEED;
    }
    if (velocity_ < -MAX_REVERSE_SPEED) {
        velocity_ = -MAX_REVERSE_SPEED;
    }

    // Update position based on velocity and rotation
    float dx = std::sin(rotation_) * velocity_ * deltaTime;
    float dz = std::cos(rotation_) * velocity_ * deltaTime;
    position_[0] = position_[0] + dx;
    position_[2] = position_[2] + dz;

    // Reset acceleration (must be reapplied each frame)
    acceleration_ = 0.0f;
}

void Vehicle::reset() {
    GameObject::reset();
    velocity_ = 0.0f;
    acceleration_ = 0.0f;
}

float Vehicle::getVelocity() const {
    return velocity_;
}

float Vehicle::getMaxSpeed() const {
    return MAX_SPEED;
}
