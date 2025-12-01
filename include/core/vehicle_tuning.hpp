#pragma once

#include <numbers>
#include <array>
#include "object_sizes.hpp"

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

// Vehicle dimensions (using common object sizes)
inline constexpr float VEHICLE_WIDTH = ObjectSizes::VEHICLE_WIDTH;
inline constexpr float VEHICLE_HEIGHT = ObjectSizes::VEHICLE_HEIGHT;
inline constexpr float VEHICLE_LENGTH = ObjectSizes::VEHICLE_LENGTH;

// Turn rate calculation constants
inline constexpr float TURN_RATE_MIN_SPEED = 0.3f;
inline constexpr float TURN_RATE_LOW_SPEED = 3.0f;
inline constexpr float TURN_RATE_MEDIUM_SPEED = 15.0f;
inline constexpr float DRIFT_ANGLE_MULTIPLIER = 1.2f;
inline constexpr float DRIFT_EXIT_RETENTION = 0.5f;
inline constexpr float DRIFT_DECAY_RATE = 0.95f;

// Turn rate calculation curve constants
inline constexpr float TURN_RATE_EXTREMELY_LOW_BASE = 0.05f;
inline constexpr float TURN_RATE_EXTREMELY_LOW_RANGE = 0.1f;
inline constexpr float TURN_RATE_EXTREMELY_LOW_DIVISOR = 0.2f;

inline constexpr float TURN_RATE_VERY_LOW_BASE = 0.15f;
inline constexpr float TURN_RATE_VERY_LOW_RANGE = 0.35f;
inline constexpr float TURN_RATE_VERY_LOW_DIVISOR = 2.7f;

inline constexpr float TURN_RATE_LOW_MEDIUM_BASE = 0.5f;
inline constexpr float TURN_RATE_LOW_MEDIUM_RANGE = 0.5f;
inline constexpr float TURN_RATE_LOW_MEDIUM_DIVISOR = 12.0f;

inline constexpr float TURN_RATE_HIGH_SPEED_BASE = 1.0f;
inline constexpr float TURN_RATE_HIGH_SPEED_REDUCTION = 0.4f;
inline constexpr float TURN_RATE_HIGH_SPEED_MIN = 0.6f;
inline constexpr float TURN_RATE_HIGH_SPEED_MAX = 1.0f;

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

// Compile-time validation of array sizes
static_assert(sizeof(GEAR_SPEEDS) / sizeof(GEAR_SPEEDS[0]) == NUM_GEARS + 1,
              "GEAR_SPEEDS array size must match NUM_GEARS + 1");
static_assert(sizeof(GEAR_ACCELERATION_MULTIPLIERS) / sizeof(GEAR_ACCELERATION_MULTIPLIERS[0]) == NUM_GEARS,
              "GEAR_ACCELERATION_MULTIPLIERS array size must match NUM_GEARS");

// Compile-time validation that MAX_SPEED != TURN_RATE_MEDIUM_SPEED to prevent division by zero
static_assert(MAX_SPEED != TURN_RATE_MEDIUM_SPEED,
              "MAX_SPEED must not equal TURN_RATE_MEDIUM_SPEED to prevent division by zero");

inline constexpr float PI = std::numbers::pi_v<float>;
inline constexpr float TWO_PI = 2.0f * PI;
inline constexpr float INITIAL_ROTATION_RADIANS = PI;

} // namespace VehicleTuning
