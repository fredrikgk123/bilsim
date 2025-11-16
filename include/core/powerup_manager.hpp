#pragma once

#include "core/powerup.hpp"
#include "core/vehicle.hpp"
#include "core/game_object_manager.hpp"
#include <vector>
#include <memory>

// Manages collection of powerups in the game
class PowerupManager : public GameObjectManager {
public:
    PowerupManager(int count, float playAreaSize);

    // Update all powerups
    virtual void update(float deltaTime) override;

    // Check and handle collisions with vehicle
    virtual void handleCollisions(Vehicle& vehicle) override;

    // Reset all powerups to active state
    virtual void reset() noexcept override;

    // Get count of active powerups
    virtual size_t getCount() const override;

    // Get all powerups (for rendering)
    [[nodiscard]] const std::vector<std::unique_ptr<Powerup>>& getPowerups() const noexcept;

private:
    void generatePowerups(int count, float playAreaSize);

    std::vector<std::unique_ptr<Powerup>> powerups_;
};
