#include "inputHandler.hpp"

using namespace threepp;

InputHandler::InputHandler(Vehicle& vehicle)
    : vehicle_(vehicle),
      upPressed_(false),
      downPressed_(false),
      leftPressed_(false),
      rightPressed_(false),
      shiftPressed_(false),
      resetCallback_(nullptr) {
}

void InputHandler::setResetCallback(std::function<void()> callback) {
    resetCallback_ = callback;
}

void InputHandler::onReset() {
    if (resetCallback_ != nullptr) {
        resetCallback_();
    }
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
        case Key::SPACE:
            if (shiftPressed_ == false) {
                vehicle_.activateNitrous();
                shiftPressed_ = true;
            }
            break;
        case Key::R:
            vehicle_.reset();
            // Trigger reset callback to respawn powerups
            onReset();
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
        case Key::SPACE:
            shiftPressed_ = false;
            break;
        default:
            break;
    }
}

void InputHandler::update(float deltaTime) {
    // Handle acceleration
    if (upPressed_ == true) {
        vehicle_.accelerateForward();
    } else if (downPressed_ == true) {
        vehicle_.accelerateBackward();
    }

    // Handle turning (reverse when moving backwards)
    float velocity = vehicle_.getVelocity();
    float turnDirection = 1.0f;

    if (velocity >= 0.0f) {
        turnDirection = 1.0f;
    } else {
        turnDirection = -1.0f;
    }

    if (leftPressed_ == true) {
        vehicle_.turn(deltaTime * turnDirection);
    }
    if (rightPressed_ == true) {
        vehicle_.turn(-deltaTime * turnDirection);
    }
}
