#include "inputHandler.hpp"

using namespace threepp;

InputHandler::InputHandler(Vehicle& vehicle)
    : vehicle_(vehicle),
      upPressed_(false),
      downPressed_(false),
      leftPressed_(false),
      rightPressed_(false) {
}

void InputHandler::onKeyPressed(KeyEvent evt) {
    // Support both arrow keys and WASD
    switch (evt.key) {
        case Key::UP:
        case Key::W:
            upPressed_ = true;
            break;
        case Key::DOWN:
        case Key::S:
            downPressed_ = true;
            break;
        case Key::LEFT:
        case Key::A:
            leftPressed_ = true;
            break;
        case Key::RIGHT:
        case Key::D:
            rightPressed_ = true;
            break;
        case Key::R:
            vehicle_.reset();
            break;
        default:
            break;
    }
}

void InputHandler::onKeyReleased(KeyEvent evt) {
    switch (evt.key) {
        case Key::UP:
        case Key::W:
            upPressed_ = false;
            break;
        case Key::DOWN:
        case Key::S:
            downPressed_ = false;
            break;
        case Key::LEFT:
        case Key::A:
            leftPressed_ = false;
            break;
        case Key::RIGHT:
        case Key::D:
            rightPressed_ = false;
            break;
        default:
            break;
    }
}

void InputHandler::update(float deltaTime) {
    // Handle acceleration
    if (upPressed_) {
        vehicle_.accelerateForward();
    } else if (downPressed_) {
        vehicle_.accelerateBackward();
    }

    // Handle turning (reverse when moving backwards)
    float turnDirection = (vehicle_.getVelocity() >= 0) ? 1.0f : -1.0f;

    if (leftPressed_) {
        vehicle_.turn(deltaTime * turnDirection);
    }
    if (rightPressed_) {
        vehicle_.turn(-deltaTime * turnDirection);
    }
}
