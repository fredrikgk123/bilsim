#pragma once

/**
 * @brief Read-only interface for accessing vehicle-specific state
 *
 * This interface decouples renderers, UI, and audio systems from the concrete
 * Vehicle class, allowing them to access vehicle state without tight coupling.
 *
 * Note: Position, rotation, and active state come from GameObject base class.
 */
class IVehicleState {
public:
    virtual ~IVehicleState() = default;

    // Scale (vehicle-specific, not in GameObject)
    [[nodiscard]] virtual float getScale() const noexcept = 0;

    // Movement state
    [[nodiscard]] virtual float getVelocity() const noexcept = 0;
    [[nodiscard]] virtual float getSteeringInput() const noexcept = 0;

    // Drift state
    [[nodiscard]] virtual bool isDrifting() const noexcept = 0;
    [[nodiscard]] virtual float getDriftAngle() const noexcept = 0;

    // Nitrous state
    [[nodiscard]] virtual bool hasNitrous() const noexcept = 0;
    [[nodiscard]] virtual bool isNitrousActive() const noexcept = 0;
    [[nodiscard]] virtual float getNitrousTimeRemaining() const noexcept = 0;

    // Engine state
    [[nodiscard]] virtual int getCurrentGear() const noexcept = 0;
    [[nodiscard]] virtual float getRPM() const noexcept = 0;
};

