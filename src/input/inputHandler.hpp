#pragma once

#include <threepp/threepp.hpp>
#include "vehicle.hpp"

class InputHandler : public threepp::KeyListener {
public:
    InputHandler(Vehicle& vehicle);

    void onKeyPressed(threepp::KeyEvent evt) override;
    void onKeyReleased(threepp::KeyEvent evt) override;

    void update(float deltaTime);

private:
    Vehicle& vehicle_;
    bool upPressed_ = false;
    bool downPressed_ = false;
    bool leftPressed_ = false;
    bool rightPressed_ = false;
};
