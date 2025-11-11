#include "powerup.hpp"
#include "object_sizes.hpp"
#include "vehicle_tuning.hpp"
#include <cmath>

namespace {
    constexpr float POWERUP_ROTATION_SPEED = 2.0f;
}

Powerup::Powerup(float x, float y, float z, PowerupType type)
    : GameObject(x, y, z),
      type_(type),
      rotationSpeed_(POWERUP_ROTATION_SPEED) {
    // Set powerup size
    size_[0] = ObjectSizes::POWERUP_SIZE;
    size_[1] = ObjectSizes::POWERUP_SIZE;
    size_[2] = ObjectSizes::POWERUP_SIZE;
}

void Powerup::update(float deltaTime) {
    // Rotate powerup for visual effect
    if (active_) {
        rotation_ += rotationSpeed_ * deltaTime;

        // Keep rotation in [0, 2π]
        rotation_ = std::fmod(rotation_, VehicleTuning::TWO_PI);
    }
}

PowerupType Powerup::getType() const noexcept {
    return type_;
}
