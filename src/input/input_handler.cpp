#include "input_handler.hpp"

using namespace threepp;

InputHandler::InputHandler(Vehicle& vehicle, SceneManager& sceneManager)
    : vehicle_(vehicle),
      sceneManager_(sceneManager),
      upPressed_(false),
      downPressed_(false),
      leftPressed_(false),
      rightPressed_(false),
      shiftPressed_(false),
      resetCallback_(nullptr) {
}

void InputHandler::setResetCallback(std::function<void()> callback) {
    resetCallback_ = std::move(callback);
}

void InputHandler::onReset() {
    if (resetCallback_) {
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
            vehicle_.startDrift();
            break;
        case Key::F:
            if (!shiftPressed_) {
                vehicle_.activateNitrous();
                shiftPressed_ = true;
            }
            break;
        case Key::C:
            // Toggle camera mode
            sceneManager_.toggleCameraMode();
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
            vehicle_.stopDrift();
            break;
        case Key::F:
            shiftPressed_ = false;
            break;
        default:
            break;
    }
}

void InputHandler::update(float deltaTime) {
    // Handle acceleration
    if (upPressed_) {
        // Vehicle now owns the acceleration multiplier; use the simple API
        vehicle_.accelerateForward();
    } else if (downPressed_) {
        vehicle_.accelerateBackward();
    }

    // Handle turning (reverse when moving backwards)
    float velocity = vehicle_.getVelocity();
    float turnDirection = (velocity >= 0.0f) ? 1.0f : -1.0f;

    if (leftPressed_) {
        vehicle_.turn(deltaTime * turnDirection);
    }
    if (rightPressed_) {
        vehicle_.turn(-deltaTime * turnDirection);
    }
}
