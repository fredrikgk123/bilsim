#pragma once

#include "graphics/game_object_renderer.hpp"
#include "core/powerup.hpp"

class PowerupRenderer : public GameObjectRenderer {
public:
    PowerupRenderer(threepp::Scene& scene, const Powerup& powerup);

protected:
    void createModel() override;

private:
    const Powerup& powerup_;
};
