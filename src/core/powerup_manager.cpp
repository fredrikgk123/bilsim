#include "core/powerup_manager.hpp"
#include "core/game_config.hpp"
#include "core/random_position_generator.hpp"


PowerupManager::PowerupManager(int count, float playAreaSize) {
    generatePowerups(count, playAreaSize);
}

void PowerupManager::generatePowerups(int count, float playAreaSize) {
    // Clear any existing powerups
    powerups_.clear();

    // Create position generator with spawn margin
    RandomPositionGenerator posGen(playAreaSize, GameConfig::Powerup::SPAWN_MARGIN);

    // Generate random powerups
    for (int i = 0; i < count; ++i) {
        auto pos = posGen.getRandomPosition();

        // For now, all powerups are NITROUS type
        auto powerup = std::make_unique<Powerup>(pos[0], GameConfig::Powerup::HEIGHT, pos[1], PowerupType::NITROUS);
        powerups_.push_back(std::move(powerup));
    }
}

void PowerupManager::update(float deltaTime) {
    for (auto& powerup : powerups_) {
        powerup->update(deltaTime);
    }
}

void PowerupManager::handleCollisions(Vehicle& vehicle) {
    for (auto& powerup : powerups_) {
        // Check collision with vehicle
        // Only allow pickup if:
        // 1. Powerup is active (not already collected)
        // 2. Vehicle doesn't have nitrous stored
        // 3. Vehicle is not currently using nitrous
        // 4. Vehicle is colliding with the powerup
        if (powerup->isActive() &&
            !vehicle.hasNitrous() &&
            !vehicle.isNitrousActive() &&
            vehicle.intersects(*powerup)) {
            vehicle.pickupNitrous();
            powerup->setActive(false);
        }
    }
}

void PowerupManager::reset() noexcept {
    // Respawn all powerups by setting them to active
    for (auto& powerup : powerups_) {
        powerup->setActive(true);
    }
}

const std::vector<std::unique_ptr<Powerup>>& PowerupManager::getPowerups() const noexcept {
    return powerups_;
}

size_t PowerupManager::getCount() const {
    return powerups_.size();
}
