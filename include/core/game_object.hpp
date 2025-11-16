#pragma once

#include <array>

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
    [[nodiscard]] virtual const std::array<float, 3>& getPosition() const noexcept;
    [[nodiscard]] virtual float getRotation() const noexcept;
    [[nodiscard]] const std::array<float, 3>& getSize() const noexcept;
    [[nodiscard]] virtual bool isActive() const noexcept;

    // Setters
    void setPosition(float x, float y, float z) noexcept;
    void setRotation(float rotation) noexcept;
    void setActive(bool active) noexcept;

    // Collision detection - circle-based for simplicity and consistency
    [[nodiscard]] bool checkCircleCollision(const GameObject& other, float& overlapDistance, float& normalX, float& normalZ) const noexcept;

    // Simple collision check without detailed information
    [[nodiscard]] bool intersects(const GameObject& other) const noexcept;

protected:
    // Position and orientation
    std::array<float, 3> position_;
    std::array<float, 3> initialPosition_;
    float rotation_;
    float initialRotation_;

    // Size for collision detection (width, height, length)
    std::array<float, 3> size_;

    // Active state
    bool active_;
};
