#pragma once

#include <memory>
#include <vector>
#include "core/game_object.hpp"

class GameObjectManager {
public:
    virtual ~GameObjectManager() = default;

    // Update all managed objects
    virtual void update(float deltaTime) = 0;

    // Handle collisions with the vehicle
    virtual void handleCollisions(class Vehicle& vehicle) = 0;

    // Reset all objects to initial state
    virtual void reset() = 0;

    // Get the number of active objects
    virtual size_t getCount() const = 0;
};
