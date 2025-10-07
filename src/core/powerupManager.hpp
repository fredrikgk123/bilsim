#pragma once

#include "powerup.hpp"
#include "vehicle.hpp"
#include <vector>
#include <memory>
#include <random>

// Manages collection of powerups in the game
class PowerupManager {
public:
    PowerupManager(int count, float playAreaSize);

    // Update all powerups
    void update(float deltaTime);

    // Check and handle collisions with vehicle
    void handleCollisions(Vehicle& vehicle);

    // Reset all powerups to active state
    void reset();

    // Get all powerups (for rendering)
    const std::vector<std::unique_ptr<Powerup>>& getPowerups() const;

private:
    void generatePowerups(int count, float playAreaSize);

    std::vector<std::unique_ptr<Powerup>> powerups_;
    std::mt19937 randomEngine_;
};

