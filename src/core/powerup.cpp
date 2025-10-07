#include "powerup.hpp"
#include <cmath>

namespace {
    const float POWERUP_SIZE = 0.8f;
    const float POWERUP_ROTATION_SPEED = 2.0f;
}

Powerup::Powerup(float x, float y, float z, PowerupType type)
    : GameObject(x, y, z),
      type_(type),
      rotationSpeed_(POWERUP_ROTATION_SPEED) {
    // Set powerup size
    size_[0] = POWERUP_SIZE;
    size_[1] = POWERUP_SIZE;
    size_[2] = POWERUP_SIZE;
}

void Powerup::update(float deltaTime) {
    // Rotate powerup for visual effect
    if (active_ == true) {
        rotation_ = rotation_ + (rotationSpeed_ * deltaTime);

        // Keep rotation in [0, 2π]
        rotation_ = std::fmod(rotation_, 2.0f * static_cast<float>(M_PI));
    }
}

PowerupType Powerup::getType() const {
    return type_;
}
