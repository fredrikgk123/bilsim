#include "core/vehicle.hpp"
#include "core/vehicle_tuning.hpp"
#include <algorithm>
#include <cmath>

Vehicle::Vehicle(float x, float y, float z)
    : GameObject(x, y, z),
      velocity_(0.0f),
      acceleration_(0.0f),
      steeringInput_(0.0f),
      isDrifting_(false),
      driftAngle_(0.0f),
      hasNitrous_(false),
      nitrousActive_(false),
      nitrousTimeRemaining_(0.0f),
      currentGear_(1),
      rpm_(VehicleTuning::IDLE_RPM),
      accelMultiplier_(1.0f) {
    // Set vehicle-specific size
    size_[0] = VehicleTuning::VEHICLE_WIDTH;
    size_[1] = VehicleTuning::VEHICLE_HEIGHT;
    size_[2] = VehicleTuning::VEHICLE_LENGTH;

    // Set initial rotation to 180 degrees (π radians) so vehicle faces down in minimap
    rotation_ = VehicleTuning::INITIAL_ROTATION_RADIANS;
    initialRotation_ = VehicleTuning::INITIAL_ROTATION_RADIANS;
}

void Vehicle::accelerateForward() noexcept {
    // Use the vehicle-owned acceleration multiplier
    accelerateForward(accelMultiplier_);
}

void Vehicle::accelerateForward(float multiplier) noexcept {
    float baseAcceleration = nitrousActive_ ? VehicleTuning::NITROUS_ACCELERATION : VehicleTuning::FORWARD_ACCELERATION;
    // Apply gear acceleration multiplier and external multiplier for tunable responsiveness
    acceleration_ = baseAcceleration * getGearAccelerationMultiplier() * multiplier;
}

void Vehicle::accelerateBackward() noexcept {
    acceleration_ = VehicleTuning::BACKWARD_ACCELERATION;
}

void Vehicle::turn(float amount) noexcept {
    steeringInput_ = amount; // Store the steering input

    float turnRate = calculateTurnRate();

    // Invert turning direction when reversing for intuitive controls
    float turnDirection = (velocity_ >= 0.0f) ? 1.0f : -1.0f;
    rotation_ += amount * VehicleTuning::TURN_SPEED * turnRate * turnDirection;

    // When drifting, allow the car to build up a drift angle
    if (isDrifting_) {
        // Accumulate drift angle more aggressively
        driftAngle_ += amount * VehicleTuning::TURN_SPEED * turnRate * VehicleTuning::DRIFT_ANGLE_MULTIPLIER * turnDirection;

        // Increased max drift angle to ~60 degrees for more dramatic slides
        const float MAX_DRIFT_ANGLE = VehicleTuning::PI / 3.0f;  // 60 degrees
        driftAngle_ = (std::clamp)(driftAngle_, -MAX_DRIFT_ANGLE, MAX_DRIFT_ANGLE);
    }

    // Normalize rotation to [0, 2π]
    rotation_ = std::fmod(rotation_, VehicleTuning::TWO_PI);
    if (rotation_ < 0.0f) {
        rotation_ += VehicleTuning::TWO_PI;
    }
}

float Vehicle::calculateTurnRate() const noexcept {
    const float absoluteVelocity = std::abs(velocity_);

    // Don't turn if completely stopped
    if (absoluteVelocity < VehicleTuning::MIN_SPEED_THRESHOLD) {
        return 0.0f;
    }

    // Extremely low speeds (0.1-0.3 m/s / ~0.4-1.1 km/h): very minimal turning
    if (absoluteVelocity < VehicleTuning::TURN_RATE_MIN_SPEED) {
        return VehicleTuning::TURN_RATE_EXTREMELY_LOW_BASE +
               ((absoluteVelocity - VehicleTuning::MIN_SPEED_THRESHOLD) / VehicleTuning::TURN_RATE_EXTREMELY_LOW_DIVISOR) *
               VehicleTuning::TURN_RATE_EXTREMELY_LOW_RANGE;
    }

    // Very low speeds (0.3-3 m/s / ~1.1-11 km/h): minimal but usable turning
    if (absoluteVelocity < VehicleTuning::TURN_RATE_LOW_SPEED) {
        return VehicleTuning::TURN_RATE_VERY_LOW_BASE +
               ((absoluteVelocity - VehicleTuning::TURN_RATE_MIN_SPEED) / VehicleTuning::TURN_RATE_VERY_LOW_DIVISOR) *
               VehicleTuning::TURN_RATE_VERY_LOW_RANGE;
    }

    // Low to medium speeds (3-15 m/s / ~11-54 km/h): good turning capability
    if (absoluteVelocity < VehicleTuning::TURN_RATE_MEDIUM_SPEED) {
        return VehicleTuning::TURN_RATE_LOW_MEDIUM_BASE +
               ((absoluteVelocity - VehicleTuning::TURN_RATE_LOW_SPEED) / VehicleTuning::TURN_RATE_LOW_MEDIUM_DIVISOR) *
               VehicleTuning::TURN_RATE_LOW_MEDIUM_RANGE;
    }

    // High speeds (15+ m/s / 54+ km/h): reduced turn rate for realism
    const float speedRatio = (absoluteVelocity - VehicleTuning::TURN_RATE_MEDIUM_SPEED) / (VehicleTuning::MAX_SPEED - VehicleTuning::TURN_RATE_MEDIUM_SPEED);
    const float turnRate = VehicleTuning::TURN_RATE_HIGH_SPEED_BASE - (speedRatio * VehicleTuning::TURN_RATE_HIGH_SPEED_REDUCTION);

    return (std::clamp)(turnRate, VehicleTuning::TURN_RATE_HIGH_SPEED_MIN, VehicleTuning::TURN_RATE_HIGH_SPEED_MAX);
}

void Vehicle::activateNitrous() noexcept {
    if (hasNitrous_ && !nitrousActive_) {
        nitrousActive_ = true;
        nitrousTimeRemaining_ = VehicleTuning::NITROUS_DURATION;
        hasNitrous_ = false; // Consumed when activated
    }
}

void Vehicle::startDrift() noexcept {
    isDrifting_ = true;
}

void Vehicle::stopDrift() noexcept {
    isDrifting_ = false;
    // Keep more of the drift angle when exiting for a smoother transition
    driftAngle_ *= VehicleTuning::DRIFT_EXIT_RETENTION;
}

bool Vehicle::isDrifting() const noexcept {
    return isDrifting_;
}

void Vehicle::pickupNitrous() noexcept {
    hasNitrous_ = true;
}

bool Vehicle::hasNitrous() const noexcept {
    return hasNitrous_;
}

bool Vehicle::isNitrousActive() const noexcept {
    return nitrousActive_;
}

float Vehicle::getNitrousTimeRemaining() const noexcept {
    return nitrousTimeRemaining_;
}

void Vehicle::update(float deltaTime) {
    updateNitrous(deltaTime);
    updateGearShifting();
    updateVelocity(deltaTime);
    updateRPM();
    updateDrift(deltaTime);
    updatePosition(deltaTime);
    decayAcceleration();
}

void Vehicle::updateNitrous(float deltaTime) noexcept {
    if (nitrousActive_) {
        nitrousTimeRemaining_ -= deltaTime;
        if (nitrousTimeRemaining_ <= 0.0f) {
            nitrousActive_ = false;
            nitrousTimeRemaining_ = 0.0f;
        }
    }
}

void Vehicle::updateVelocity(float deltaTime) noexcept {
    // Update velocity based on acceleration
    velocity_ += acceleration_ * deltaTime;

    // Apply friction (less friction while drifting)
    // Logarithmic friction curve: more friction at low speeds, less at high speeds
    float baseFriction = isDrifting_ ? VehicleTuning::DRIFT_FRICTION_COEFFICIENT : VehicleTuning::FRICTION_COEFFICIENT;

    // Calculate friction multiplier using logarithmic curve
    float speedRatio = std::abs(velocity_) / VehicleTuning::MAX_SPEED;
    speedRatio = (std::clamp)(speedRatio, 0.01f, 1.0f); // Prevent log(0)

    // Logarithmic curve: friction increases as speed decreases
    float logValue = std::log(speedRatio);
    float frictionRange = VehicleTuning::FRICTION_COEFFICIENT - 0.994f;
    float frictionMultiplier = 0.994f + ((logValue + 4.6f) / 4.6f) * frictionRange;
    frictionMultiplier = (std::clamp)(frictionMultiplier, 0.994f, VehicleTuning::FRICTION_COEFFICIENT);

    float frictionCoefficient = isDrifting_ ? baseFriction : frictionMultiplier;
    velocity_ *= frictionCoefficient;

    // Clamp velocity to max speeds (higher during nitrous)
    float currentMaxSpeed = nitrousActive_ ? VehicleTuning::NITROUS_MAX_SPEED : VehicleTuning::MAX_SPEED;
    velocity_ = std::clamp(velocity_, -VehicleTuning::MAX_REVERSE_SPEED, currentMaxSpeed);
}

void Vehicle::updateRPM() noexcept {
    float absoluteVelocity = std::abs(velocity_);
    if (absoluteVelocity < 0.1f) {
        // Idle RPM when stopped
        rpm_ = VehicleTuning::IDLE_RPM;
    } else if (currentGear_ > 0 && currentGear_ <= VehicleTuning::NUM_GEARS) {
        // Calculate RPM based on speed within current gear's range
        float gearMinSpeed = VehicleTuning::GEAR_SPEEDS[currentGear_ - 1];
        float gearMaxSpeed = VehicleTuning::GEAR_SPEEDS[currentGear_];
        float speedRatio = (absoluteVelocity - gearMinSpeed) / (gearMaxSpeed - gearMinSpeed);
        speedRatio = std::clamp(speedRatio, 0.0f, 1.0f);

        // Map speed ratio to RPM range (shift point to max RPM)
        rpm_ = VehicleTuning::GEAR_SHIFT_DOWN_RPM + speedRatio * (VehicleTuning::MAX_RPM - VehicleTuning::GEAR_SHIFT_DOWN_RPM);
    }
}

void Vehicle::updateDrift(float deltaTime) noexcept {
    if (isDrifting_) {
        // Gradually reduce drift angle over time (self-correcting)
        driftAngle_ *= VehicleTuning::DRIFT_DECAY_RATE;
    }
}

void Vehicle::updatePosition(float deltaTime) noexcept {
    // When drifting, car moves in a direction between facing and drift angle
    float movementAngle = rotation_;
    if (isDrifting_) {
        movementAngle = rotation_ - driftAngle_;
    }

    // Update position based on velocity and movement angle
    const float deltaX = std::sin(movementAngle) * velocity_ * deltaTime;
    const float deltaZ = std::cos(movementAngle) * velocity_ * deltaTime;
    position_[0] += deltaX;
    position_[2] += deltaZ;
}

void Vehicle::decayAcceleration() noexcept {
    // Reset acceleration (must be reapplied each frame)
    acceleration_ = 0.0f;

    // Decay steering input towards zero (natural return to center)
    steeringInput_ *= 0.85f;
    if (std::abs(steeringInput_) < 0.01f) {
        steeringInput_ = 0.0f;
    }
}

void Vehicle::reset() {
    GameObject::reset();
    velocity_ = 0.0f;
    acceleration_ = 0.0f;
    steeringInput_ = 0.0f;
    isDrifting_ = false;
    driftAngle_ = 0.0f;
    hasNitrous_ = false;
    nitrousActive_ = false;
    nitrousTimeRemaining_ = 0.0f;
    currentGear_ = 1;
    rpm_ = VehicleTuning::IDLE_RPM;

    // Reset camera to follow mode
    if (resetCameraCallback_) {
        resetCameraCallback_();
    }
}

void Vehicle::setResetCameraCallback(std::function<void()>&& callback) noexcept {
    resetCameraCallback_ = std::move(callback);
}

float Vehicle::getVelocity() const noexcept {
    return velocity_;
}

float Vehicle::getMaxSpeed() noexcept {
    return VehicleTuning::MAX_SPEED;
}

void Vehicle::setVelocity(float velocity) noexcept {
    // Clamp velocity to reasonable bounds to prevent physics bugs
    const float MAX_VELOCITY = VehicleTuning::MAX_SPEED * 1.5f;  // Allow slight overspeed
    velocity_ = std::clamp(velocity, -MAX_VELOCITY, MAX_VELOCITY);
}

float Vehicle::getDriftAngle() const noexcept {
    return driftAngle_;
}

int Vehicle::getCurrentGear() const noexcept {
    return currentGear_;
}

float Vehicle::getRPM() const noexcept {
    return rpm_;
}

float Vehicle::getSteeringInput() const noexcept {
    return steeringInput_;
}

void Vehicle::updateGearShifting() noexcept {
    float absoluteVelocity = std::abs(velocity_);

    // Don't shift during reverse
    if (velocity_ < 0.0f) {
        currentGear_ = 0; // Reverse gear
        return;
    }

    // Start in gear 1 when moving forward from stop
    if (absoluteVelocity < 0.1f) {
        currentGear_ = 1;
        return;
    }

    // Automatic gear shifting based on speed
    // Shift up when reaching the upper speed threshold for current gear
    if (currentGear_ < VehicleTuning::NUM_GEARS && absoluteVelocity >= VehicleTuning::GEAR_SPEEDS[currentGear_]) {
        currentGear_++;
    }
    // Shift down when falling below the lower speed threshold
    else if (currentGear_ > 1 && absoluteVelocity < VehicleTuning::GEAR_SPEEDS[currentGear_ - 1]) {
        currentGear_--;
    }
}

float Vehicle::getGearAccelerationMultiplier() const noexcept {
    // Return multiplier based on current gear (lower gears = more torque)
    if (currentGear_ >= 1 && currentGear_ <= VehicleTuning::NUM_GEARS) {
        return VehicleTuning::GEAR_ACCELERATION_MULTIPLIERS[currentGear_ - 1];
    }
    return 1.0f; // Default multiplier for reverse or invalid gear
}

void Vehicle::setScale(float scale) noexcept {
    // Clamp to reasonable values
    if (scale <= 0.0f) scale = 1.0f;
    scale_ = scale;

    // Update collision/size used by renderers
    size_[0] = VehicleTuning::VEHICLE_WIDTH * scale_;
    size_[1] = VehicleTuning::VEHICLE_HEIGHT * scale_;
    size_[2] = VehicleTuning::VEHICLE_LENGTH * scale_;
}

[[nodiscard]] float Vehicle::getScale() const noexcept {
    return scale_;
}
