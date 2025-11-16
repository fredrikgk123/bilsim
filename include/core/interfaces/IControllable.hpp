#pragma once

/**
 * @brief Interface for controlling a game entity via input
 *
 * This interface decouples the input handler from concrete entity classes,
 * allowing the input system to control any object that implements this interface.
 */
class IControllable {
public:
    virtual ~IControllable() = default;

    // Movement controls
    virtual void accelerateForward() noexcept = 0;
    virtual void accelerateBackward() noexcept = 0;
    virtual void turn(float amount) noexcept = 0;

    // Special actions
    virtual void startDrift() noexcept = 0;
    virtual void stopDrift() noexcept = 0;
    virtual void activateNitrous() noexcept = 0;

    // State management
    virtual void reset() = 0;
};

