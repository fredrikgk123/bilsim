#include "vehicle.hpp"
#include <cmath>

// Anonymous namespace - these constants are LOCAL to this file only (NOT global!)
namespace {
    // Physics constants (realistic tuning)
    constexpr float MAX_SPEED = 41.67f;                  // ~150 km/h (realistic for a small car)
    constexpr float MAX_REVERSE_SPEED = 13.9f;           // ~50 km/h reverse
    constexpr float TURN_SPEED = 1.5f;                   // 1.5 rad/sec (~86°/sec) - sharper turning
    constexpr float FORWARD_ACCELERATION = 8.0f;         // Balanced acceleration
    constexpr float BACKWARD_ACCELERATION = -4.0f;       // Slower reverse acceleration
    constexpr float FRICTION_COEFFICIENT = 0.997f;       // More friction for tighter control
    constexpr float DRIFT_FRICTION_COEFFICIENT = 0.992f; // Less friction while drifting
    constexpr float MIN_SPEED_THRESHOLD = 0.1f;          // Minimum speed for any turning

    // Nitrous constants
    constexpr float NITROUS_DURATION = 5.0f;             // 5 seconds of boost
    constexpr float NITROUS_ACCELERATION = 14.0f;        // Moderate boost acceleration
    constexpr float NITROUS_MAX_SPEED = 50.0f;           // ~180 km/h during boost

    // Vehicle dimensions
    constexpr float VEHICLE_WIDTH = 1.0f;
    constexpr float VEHICLE_HEIGHT = 0.5f;
    constexpr float VEHICLE_LENGTH = 2.0f;

    // Turn rate calculation constants
    constexpr float TURN_RATE_MIN_SPEED = 0.3f;          // Speed threshold for minimal turning
    constexpr float TURN_RATE_LOW_SPEED = 3.0f;          // Speed threshold for low-speed turning
    constexpr float TURN_RATE_MEDIUM_SPEED = 15.0f;      // Speed threshold for medium-speed turning
    constexpr float DRIFT_ANGLE_MULTIPLIER = 1.2f;       // Drift angle accumulation rate
    constexpr float DRIFT_EXIT_RETENTION = 0.5f;         // How much drift angle to keep when exiting drift
    constexpr float DRIFT_DECAY_RATE = 0.95f;            // Drift angle decay per frame
}

Vehicle::Vehicle(float x, float y, float z)
    : GameObject(x, y, z),
      velocity_(0.0f),
      acceleration_(0.0f),
      isDrifting_(false),
      driftAngle_(0.0f),
      hasNitrous_(false),
      nitrousActive_(false),
      nitrousTimeRemaining_(0.0f) {
    // Set vehicle-specific size
    size_[0] = VEHICLE_WIDTH;
    size_[1] = VEHICLE_HEIGHT;
    size_[2] = VEHICLE_LENGTH;

    // Set initial rotation to 180 degrees (π radians) so minimap direction matches
    rotation_ = static_cast<float>(M_PI);
    initialRotation_ = static_cast<float>(M_PI);
}

void Vehicle::accelerateForward() {
    acceleration_ = nitrousActive_ ? NITROUS_ACCELERATION : FORWARD_ACCELERATION;
}

void Vehicle::accelerateBackward() {
    acceleration_ = BACKWARD_ACCELERATION;
}

void Vehicle::turn(float amount) {
    float turnRate = calculateTurnRate();
    rotation_ += amount * TURN_SPEED * turnRate;

    // When drifting, allow the car to build up a drift angle
    if (isDrifting_) {
        // Accumulate drift angle more aggressively
        driftAngle_ += amount * TURN_SPEED * turnRate * DRIFT_ANGLE_MULTIPLIER;

        // Increased max drift angle to ~60 degrees for more dramatic slides
        const float MAX_DRIFT_ANGLE = static_cast<float>(M_PI) / 3.0f;
        driftAngle_ = std::clamp(driftAngle_, -MAX_DRIFT_ANGLE, MAX_DRIFT_ANGLE);
    }

    // Normalize rotation to [0, 2π]
    rotation_ = std::fmod(rotation_, 2.0f * static_cast<float>(M_PI));
    if (rotation_ < 0.0f) {
        rotation_ += 2.0f * static_cast<float>(M_PI);
    }
}

float Vehicle::calculateTurnRate() const {
    const float absoluteVelocity = std::abs(velocity_);

    // Don't turn if completely stopped
    if (absoluteVelocity < MIN_SPEED_THRESHOLD) {
        return 0.0f;
    }

    // Extremely low speeds (0.1-0.3 m/s / ~0.4-1.1 km/h): very minimal turning
    if (absoluteVelocity < TURN_RATE_MIN_SPEED) {
        return 0.05f + ((absoluteVelocity - MIN_SPEED_THRESHOLD) / 0.2f) * 0.1f;
    }

    // Very low speeds (0.3-3 m/s / ~1.1-11 km/h): minimal but usable turning
    if (absoluteVelocity < TURN_RATE_LOW_SPEED) {
        return 0.15f + ((absoluteVelocity - TURN_RATE_MIN_SPEED) / 2.7f) * 0.35f;
    }

    // Low to medium speeds (3-15 m/s / ~11-54 km/h): good turning capability
    if (absoluteVelocity < TURN_RATE_MEDIUM_SPEED) {
        return 0.5f + ((absoluteVelocity - TURN_RATE_LOW_SPEED) / 12.0f) * 0.5f;
    }

    // High speeds (15+ m/s / 54+ km/h): reduced turn rate for realism
    const float speedRatio = (absoluteVelocity - TURN_RATE_MEDIUM_SPEED) / (MAX_SPEED - TURN_RATE_MEDIUM_SPEED);
    const float turnRate = 1.0f - (speedRatio * 0.4f);  // Reduces to 60% at max speed

    return std::clamp(turnRate, 0.6f, 1.0f);
}

void Vehicle::activateNitrous() {
    if (hasNitrous_ && !nitrousActive_) {
        nitrousActive_ = true;
        nitrousTimeRemaining_ = NITROUS_DURATION;
        hasNitrous_ = false; // Consumed when activated
    }
}

void Vehicle::startDrift() {
    isDrifting_ = true;
}

void Vehicle::stopDrift() {
    isDrifting_ = false;
    // Keep more of the drift angle when exiting for a smoother transition
    driftAngle_ *= DRIFT_EXIT_RETENTION;
}

bool Vehicle::isDrifting() const {
    return isDrifting_;
}

void Vehicle::pickupNitrous() {
    hasNitrous_ = true;
}

bool Vehicle::hasNitrous() const {
    return hasNitrous_;
}

bool Vehicle::isNitrousActive() const {
    return nitrousActive_;
}

float Vehicle::getNitrousTimeRemaining() const {
    return nitrousTimeRemaining_;
}

void Vehicle::update(float deltaTime) {
    // Update nitrous timer
    if (nitrousActive_) {
        nitrousTimeRemaining_ -= deltaTime;
        if (nitrousTimeRemaining_ <= 0.0f) {
            nitrousActive_ = false;
            nitrousTimeRemaining_ = 0.0f;
        }
    }

    // Update velocity based on acceleration
    velocity_ += acceleration_ * deltaTime;

    // Apply friction (less friction while drifting)
    float frictionCoefficient = isDrifting_ ? DRIFT_FRICTION_COEFFICIENT : FRICTION_COEFFICIENT;
    velocity_ *= frictionCoefficient;

    // Clamp velocity to max speeds (higher during nitrous)
    float currentMaxSpeed = nitrousActive_ ? NITROUS_MAX_SPEED : MAX_SPEED;
    velocity_ = std::clamp(velocity_, -MAX_REVERSE_SPEED, currentMaxSpeed);

    // When drifting, car moves in a direction between facing and drift angle
    float movementAngle = rotation_;
    if (isDrifting_) {
        movementAngle = rotation_ - driftAngle_;
        // Gradually reduce drift angle over time (self-correcting)
        driftAngle_ *= DRIFT_DECAY_RATE;
    }

    // Update position based on velocity and movement angle
    const float deltaX = std::sin(movementAngle) * velocity_ * deltaTime;
    const float deltaZ = std::cos(movementAngle) * velocity_ * deltaTime;
    position_[0] += deltaX;
    position_[2] += deltaZ;

    // Reset acceleration (must be reapplied each frame)
    acceleration_ = 0.0f;
}

void Vehicle::reset() {
    GameObject::reset();
    velocity_ = 0.0f;
    acceleration_ = 0.0f;
    isDrifting_ = false;
    driftAngle_ = 0.0f;
    hasNitrous_ = false;
    nitrousActive_ = false;
    nitrousTimeRemaining_ = 0.0f;

    // Reset camera to follow mode
    if (resetCameraCallback_) {
        resetCameraCallback_();
    }
}

void Vehicle::setResetCameraCallback(std::function<void()>&& callback) {
    resetCameraCallback_ = std::move(callback);
}

float Vehicle::getVelocity() const {
    return velocity_;
}

float Vehicle::getMaxSpeed() const {
    return MAX_SPEED;
}
