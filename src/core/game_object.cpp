#include "gameObject.hpp"
#include <cmath>

GameObject::GameObject(float x, float y, float z)
    : position_({x, y, z}),
      initialPosition_({x, y, z}),
      rotation_(0.0f),
      initialRotation_(0.0f),
      size_({1.0f, 1.0f, 1.0f}),
      active_(true) {
}

void GameObject::reset() {
    position_ = initialPosition_;
    rotation_ = initialRotation_;
    active_ = true;
}

const std::array<float, 3>& GameObject::getPosition() const {
    return position_;
}

float GameObject::getRotation() const {
    return rotation_;
}

const std::array<float, 3>& GameObject::getSize() const {
    return size_;
}

bool GameObject::isActive() const {
    return active_;
}

void GameObject::setPosition(float x, float y, float z) {
    position_[0] = x;
    position_[1] = y;
    position_[2] = z;
}

void GameObject::setRotation(float rotation) {
    rotation_ = rotation;
}

void GameObject::setActive(bool active) {
    active_ = active;
}

bool GameObject::intersects(const GameObject& other) const {
    // Simple AABB (Axis-Aligned Bounding Box) collision detection
    if (!active_ || !other.active_) {
        return false;
    }

    // Calculate bounding box edges
    float thisMinX = position_[0] - (size_[0] / 2.0f);
    float thisMaxX = position_[0] + (size_[0] / 2.0f);
    float thisMinZ = position_[2] - (size_[2] / 2.0f);
    float thisMaxZ = position_[2] + (size_[2] / 2.0f);

    float otherMinX = other.position_[0] - (other.size_[0] / 2.0f);
    float otherMaxX = other.position_[0] + (other.size_[0] / 2.0f);
    float otherMinZ = other.position_[2] - (other.size_[2] / 2.0f);
    float otherMaxZ = other.position_[2] + (other.size_[2] / 2.0f);

    // Check for overlap on X and Z axes
    bool overlapX = (thisMinX <= otherMaxX) && (thisMaxX >= otherMinX);
    bool overlapZ = (thisMinZ <= otherMaxZ) && (thisMaxZ >= otherMinZ);

    return overlapX && overlapZ;
}
