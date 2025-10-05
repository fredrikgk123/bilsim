#pragma once

#include <threepp/threepp.hpp>
#include "vehicle.hpp"

class InputHandler : public threepp::KeyListener {
public:
    explicit InputHandler(Vehicle& vehicle);

    void onKeyPressed(threepp::KeyEvent evt) override;
    void onKeyReleased(threepp::KeyEvent evt) override;

    // Apply input to vehicle
    void update(float deltaTime);

private:
    Vehicle& vehicle_;

    // Key state tracking
    bool upPressed_;
    bool downPressed_;
    bool leftPressed_;
    bool rightPressed_;
};
