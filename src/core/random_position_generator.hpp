#pragma once

#include <array>
#include <vector>
#include <random>
#include <cmath>

/**
 * Utility class for generating random positions within a play area
 * Used by managers to spawn objects with proper spacing
 */
class RandomPositionGenerator {
public:
    RandomPositionGenerator(float playAreaSize, float margin)
        : randomEngine_(std::random_device{}()),
          minPos_(-(playAreaSize / 2.0f) + margin),
          maxPos_((playAreaSize / 2.0f) - margin),
          distribution_(minPos_, maxPos_) {
    }

    // Get a single random 2D position (x, z)
    std::array<float, 2> getRandomPosition() {
        return {distribution_(randomEngine_), distribution_(randomEngine_)};
    }

    // Get a random position that maintains minimum distance from existing positions
    std::array<float, 2> getRandomPositionWithMinDistance(
        const std::vector<std::array<float, 2>>& existingPositions,
        float minDistance,
        int maxAttempts = 100) {

        int attempts = 0;
        while (attempts < maxAttempts) {
            attempts++;
            auto pos = getRandomPosition();

            if (isPositionValid(pos, existingPositions, minDistance)) {
                return pos;
            }
        }

        // If we couldn't find a valid position, return the last attempt
        return getRandomPosition();
    }

    // Get a random position that maintains distance from center and existing positions
    std::array<float, 2> getRandomPositionWithConstraints(
        const std::vector<std::array<float, 2>>& existingPositions,
        float minDistanceFromCenter,
        float minDistanceFromOthers,
        int maxAttempts = 100) {

        int attempts = 0;
        while (attempts < maxAttempts) {
            attempts++;
            auto pos = getRandomPosition();

            // Check distance from center
            float distFromCenter = std::sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
            if (distFromCenter < minDistanceFromCenter) {
                continue;
            }

            // Check distance from existing positions
            if (isPositionValid(pos, existingPositions, minDistanceFromOthers)) {
                return pos;
            }
        }

        // If we couldn't find a valid position, return the last attempt
        return getRandomPosition();
    }

private:
    bool isPositionValid(
        const std::array<float, 2>& pos,
        const std::vector<std::array<float, 2>>& existingPositions,
        float minDistance) const {

        for (const auto& existing : existingPositions) {
            float dx = pos[0] - existing[0];
            float dz = pos[1] - existing[1];
            float distance = std::sqrt(dx * dx + dz * dz);

            if (distance < minDistance) {
                return false;
            }
        }

        return true;
    }

    std::mt19937 randomEngine_;
    float minPos_;
    float maxPos_;
    std::uniform_real_distribution<float> distribution_;
};

