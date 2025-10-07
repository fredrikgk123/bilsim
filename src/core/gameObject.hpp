#pragma once

#include <array>
#include <string>

// Base class for all game objects (vehicles, powerups, obstacles, etc.)
class GameObject {
public:
    GameObject(float x, float y, float z);
    virtual ~GameObject() = default;

    // Update method - must be implemented by derived classes
    virtual void update(float deltaTime) = 0;

    // Reset to initial state
    virtual void reset();

    // Getters
    const std::array<float, 3>& getPosition() const;
    float getRotation() const;
    const std::array<float, 3>& getSize() const;
    bool isActive() const;

    // Setters
    void setPosition(float x, float y, float z);
    void setRotation(float rotation);
    void setActive(bool active);

    // Collision detection helper
    bool intersects(const GameObject& other) const;

protected:
    // Position and orientation
    std::array<float, 3> position_;
    std::array<float, 3> initialPosition_;
    float rotation_;
    float initialRotation_;  // Store initial rotation for reset

    // Size for collision detection (width, height, length)
    std::array<float, 3> size_;

    // Active state (for object pooling and visibility)
    bool active_;
};
