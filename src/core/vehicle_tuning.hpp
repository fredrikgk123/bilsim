#pragma once

#include <numbers>

namespace VehicleTuning {

// Physics constants
inline constexpr float MAX_SPEED = 55.56f;
inline constexpr float MAX_REVERSE_SPEED = 13.9f;
inline constexpr float TURN_SPEED = 1.5f;
inline constexpr float FORWARD_ACCELERATION = 8.0f;
inline constexpr float BACKWARD_ACCELERATION = -4.0f;
inline constexpr float FRICTION_COEFFICIENT = 0.9985f;
inline constexpr float DRIFT_FRICTION_COEFFICIENT = 0.992f;
inline constexpr float MIN_SPEED_THRESHOLD = 0.1f;

// Nitrous constants
inline constexpr float NITROUS_DURATION = 5.0f;
inline constexpr float NITROUS_ACCELERATION = 14.0f;
inline constexpr float NITROUS_MAX_SPEED = 69.44f;

// Vehicle dimensions
inline constexpr float VEHICLE_WIDTH = 1.0f;
inline constexpr float VEHICLE_HEIGHT = 0.5f;
inline constexpr float VEHICLE_LENGTH = 2.0f;

// Turn rate calculation constants
inline constexpr float TURN_RATE_MIN_SPEED = 0.3f;
inline constexpr float TURN_RATE_LOW_SPEED = 3.0f;
inline constexpr float TURN_RATE_MEDIUM_SPEED = 15.0f;
inline constexpr float DRIFT_ANGLE_MULTIPLIER = 1.2f;
inline constexpr float DRIFT_EXIT_RETENTION = 0.5f;
inline constexpr float DRIFT_DECAY_RATE = 0.95f;

// Gear system constants
inline constexpr int NUM_GEARS = 5;
inline constexpr float GEAR_SHIFT_UP_RPM = 6000.0f;
inline constexpr float GEAR_SHIFT_DOWN_RPM = 2500.0f;
inline constexpr float IDLE_RPM = 1000.0f;
inline constexpr float MAX_RPM = 7000.0f;

// Speed ranges for each gear (m/s)
inline constexpr float GEAR_SPEEDS[NUM_GEARS + 1] = {
    0.0f, 12.0f, 22.0f, 35.0f, 48.0f, 70.0f
};

// Acceleration multipliers per gear
inline constexpr float GEAR_ACCELERATION_MULTIPLIERS[NUM_GEARS] = {
    1.5f, 1.2f, 1.0f, 0.8f, 0.6f
};

inline constexpr float PI = std::numbers::pi_v<float>;
inline constexpr float TWO_PI = 2.0f * PI;
inline constexpr float INITIAL_ROTATION_RADIANS = PI;

} // namespace VehicleTuning

