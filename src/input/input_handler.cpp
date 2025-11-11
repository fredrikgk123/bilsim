#include "input_handler.hpp"
#include "../core/vehicle_tuning.hpp"

using namespace threepp;


InputHandler::InputHandler(IControllable& controllable, SceneManager& sceneManager)
    : controllable_(controllable),
      sceneManager_(sceneManager),
      wPressed_(false),
      sPressed_(false),
      aPressed_(false),
      dPressed_(false),
      steerLeftPressed_(false),
      steerRightPressed_(false),
      leftArrowPressed_(false),
      rightArrowPressed_(false),
      downArrowPressed_(false),
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
    // WASD for steering, arrows for camera snaps
    switch (evt.key) {
        case Key::W:
            wPressed_ = true;
            break;
        case Key::S:
            sPressed_ = true;
            break;
        case Key::A:
            aPressed_ = true;
            steerLeftPressed_ = true;
            break;
        case Key::D:
            dPressed_ = true;
            steerRightPressed_ = true;
            break;
        case Key::LEFT:
            leftArrowPressed_ = true;
            updateCamera();
            break;
        case Key::RIGHT:
            rightArrowPressed_ = true;
            updateCamera();
            break;
        case Key::UP:
            // Reset camera forward
            sceneManager_.setCameraYaw(0.0f);
            break;
        case Key::DOWN:
            downArrowPressed_ = true;
            updateCamera();
            break;
        case Key::SPACE:
            controllable_.startDrift();
            break;
        case Key::F:
            if (!shiftPressed_) {
                controllable_.activateNitrous();
                shiftPressed_ = true;
            }
            break;
        case Key::C:
            // Toggle camera mode
            sceneManager_.toggleCameraMode();
            break;
        case Key::R:
            controllable_.reset();
            // Trigger reset callback to respawn powerups
            onReset();
            break;
        default:
            break;
    }
}

void InputHandler::onKeyReleased(KeyEvent evt) {
    switch (evt.key) {
        case Key::W:
            wPressed_ = false;
            break;
        case Key::S:
            sPressed_ = false;
            break;
        case Key::A:
            aPressed_ = false;
            steerLeftPressed_ = false;
            break;
        case Key::D:
            dPressed_ = false;
            steerRightPressed_ = false;
            break;
        case Key::LEFT:
            leftArrowPressed_ = false;
            updateCamera();
            break;
        case Key::RIGHT:
            rightArrowPressed_ = false;
            updateCamera();
            break;
        case Key::DOWN:
            downArrowPressed_ = false;
            updateCamera();
            break;
        case Key::SPACE:
            controllable_.stopDrift();
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
    if (wPressed_) {
        // Controllable entity now owns the acceleration multiplier; use the simple API
        controllable_.accelerateForward();
    } else if (sPressed_) {
        controllable_.accelerateBackward();
    }

    // Handle turning
    if (aPressed_) {
        controllable_.turn(deltaTime);
    }
    if (dPressed_) {
        controllable_.turn(-deltaTime);
    }
}

void InputHandler::updateCamera() {
    if (leftArrowPressed_) {
        sceneManager_.setCameraYawTarget(1.0f);
    } else if (rightArrowPressed_) {
        sceneManager_.setCameraYawTarget(-1.0f);
    } else if (downArrowPressed_) {
        sceneManager_.setCameraYaw(VehicleTuning::PI);
    } else {
        sceneManager_.setCameraYaw(0.0f);
    }
}
