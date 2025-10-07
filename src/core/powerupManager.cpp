#include "powerupManager.hpp"
#include <random>

PowerupManager::PowerupManager(int count, float playAreaSize)
    : randomEngine_(std::random_device{}()) {
    generatePowerups(count, playAreaSize);
}

void PowerupManager::generatePowerups(int count, float playAreaSize) {
    // Clear any existing powerups
    powerups_.clear();

    // Setup random distribution for positions
    // Keep powerups within bounds, leaving some margin from edges
    float margin = 10.0f;
    float minPos = -(playAreaSize / 2.0f) + margin;
    float maxPos = (playAreaSize / 2.0f) - margin;

    std::uniform_real_distribution<float> posDistribution(minPos, maxPos);

    // Generate random powerups
    for (int i = 0; i < count; ++i) {
        float x = posDistribution(randomEngine_);
        float z = posDistribution(randomEngine_);
        float y = 0.4f; // Fixed height above ground

        // For now, all powerups are NITROUS type
        auto powerup = std::make_unique<Powerup>(x, y, z, PowerupType::NITROUS);
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
        if (powerup->isActive() == true &&
            vehicle.hasNitrous() == false &&
            vehicle.isNitrousActive() == false &&
            vehicle.intersects(*powerup) == true) {
            vehicle.pickupNitrous();
            powerup->setActive(false);
        }
    }
}

void PowerupManager::reset() {
    for (auto& powerup : powerups_) {
        powerup->reset();
    }
}

const std::vector<std::unique_ptr<Powerup>>& PowerupManager::getPowerups() const {
    return powerups_;
}

