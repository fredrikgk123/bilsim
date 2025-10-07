#pragma once

#include "gameObject.hpp"

enum class PowerupType {
    NITROUS
};

class Powerup : public GameObject {
public:
    Powerup(float x, float y, float z, PowerupType type);

    void update(float deltaTime) override;

    PowerupType getType() const;

private:
    PowerupType type_;
    float rotationSpeed_;
};

