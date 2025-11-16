#pragma once

#include "core/game_object.hpp"

enum class PowerupType {
    NITROUS
};

class Powerup : public GameObject {
public:
    Powerup(float x, float y, float z, PowerupType type);

    void update(float deltaTime) override;

    [[nodiscard]] PowerupType getType() const noexcept;

private:
    PowerupType type_;
    float rotationSpeed_;
};
