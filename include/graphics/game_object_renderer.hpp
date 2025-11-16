#pragma once

#include <threepp/threepp.hpp>
#include <memory>
#include "core/game_object.hpp"

// Generic renderer that can render any GameObject
class GameObjectRenderer {
public:
    GameObjectRenderer(threepp::Scene& scene, const GameObject& gameObject);
    virtual ~GameObjectRenderer();

    // Update visual representation to match game object state
    virtual void update();

    // Change visibility
    void setVisible(bool visible);

protected:
    // Override this to create custom models for different object types
    virtual void createModel();

    // Protected members that derived classes can access
    threepp::Scene& scene_;
    const GameObject& gameObject_;
    std::shared_ptr<threepp::Group> objectGroup_;
    std::shared_ptr<threepp::Mesh> bodyMesh_;
};
