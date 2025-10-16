#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "audioManager.hpp"
#include "../core/vehicle.hpp"
#include <iostream>
#include <cmath>

namespace {
    // Audio volume constants
    constexpr float ENGINE_IDLE_VOLUME = 0.3f;        // 30% volume at idle
    constexpr float ENGINE_IDLE_PITCH = 0.8f;         // 80% pitch at idle (lower for idle sound)
    constexpr float ENGINE_MAX_VOLUME = 0.8f;         // 80% max volume at full throttle
    constexpr float NITROUS_VOLUME_BOOST = 0.3f;      // Add 30% volume during nitrous
    constexpr float NITROUS_PITCH_MULTIPLIER = 1.2f;  // 20% higher pitch during nitrous
    constexpr float MAX_VOLUME = 1.0f;                // Cap at 100% to prevent distortion

    constexpr float DRIFT_SOUND_VOLUME = 0.4f;        // 40% volume for drift sound
    constexpr float DRIFT_SOUND_MIN_VOLUME = 0.3f;
    constexpr float DRIFT_SOUND_MAX_VOLUME = 0.6f;
    constexpr float DRIFT_MIN_SPEED = 5.0f;           // Minimum speed for drift sound

    // Audio pitch constants
    constexpr float ENGINE_PITCH_MIN = 0.8f;          // Minimum pitch (idle)
    constexpr float ENGINE_PITCH_MAX = 2.0f;          // Maximum pitch (max RPM)
    constexpr float ENGINE_PITCH_RANGE = ENGINE_PITCH_MAX - ENGINE_PITCH_MIN; // 1.2

    // Reference speeds for audio calculation
    constexpr float BASE_REFERENCE_SPEED = 20.0f;     // Base speed for pitch calculation
    constexpr float NITROUS_REFERENCE_SPEED = 25.0f;  // Higher reference during nitrous

    // Asset path
    const std::string DRIFT_SOUND_PATH = "assets/tireScreech.wav";
}

// Implement custom deleters
void AudioManager::AudioDeleter::operator()(ma_engine* engine) const {
    if (engine) {
        ma_engine_uninit(engine);
        delete engine;
    }
}

void AudioManager::AudioDeleter::operator()(ma_sound* sound) const {
    if (sound) {
        ma_sound_uninit(sound);
        delete sound;
    }
}

AudioManager::AudioManager()
    : engine_(nullptr),
      engineSound_(nullptr),
      driftSound_(nullptr),
      initialized_(false),
      soundLoaded_(false),
      driftSoundLoaded_(false) {
}

AudioManager::~AudioManager() = default;

bool AudioManager::initialize(const std::string& engineSoundPath) {
    // Initialize audio engine - use unique_ptr from the start for exception safety
    std::unique_ptr<ma_engine, AudioDeleter> engine(new ma_engine());
    ma_result result = ma_engine_init(nullptr, engine.get());

    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine" << std::endl;
        return false;
    }
    engine_ = std::move(engine);
    initialized_ = true;

    // Load engine sound file - use unique_ptr from the start
    std::unique_ptr<ma_sound, AudioDeleter> sound(new ma_sound());
    result = ma_sound_init_from_file(engine_.get(), engineSoundPath.c_str(),
                                     MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION,
                                     nullptr, nullptr, sound.get());

    if (result != MA_SUCCESS) {
        std::cout << "Engine sound not found at: " << engineSoundPath << std::endl;
        return false;
    }
    engineSound_ = std::move(sound);

    // Configure sound playback
    ma_sound_set_looping(engineSound_.get(), MA_TRUE);
    ma_sound_set_volume(engineSound_.get(), ENGINE_IDLE_VOLUME);
    ma_sound_set_pitch(engineSound_.get(), ENGINE_IDLE_PITCH);
    ma_sound_start(engineSound_.get());

    soundLoaded_ = true;

    // Load drift/tire screech sound file
    std::unique_ptr<ma_sound, AudioDeleter> driftSnd(new ma_sound());
    result = ma_sound_init_from_file(engine_.get(), DRIFT_SOUND_PATH.c_str(),
                                     MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION,
                                     nullptr, nullptr, driftSnd.get());

    if (result != MA_SUCCESS) {
        std::cout << "Drift sound not found at: " << DRIFT_SOUND_PATH << std::endl;
        // driftSnd will be automatically cleaned up
    } else {
        driftSound_ = std::move(driftSnd);
        ma_sound_set_looping(driftSound_.get(), MA_TRUE);
        ma_sound_set_volume(driftSound_.get(), DRIFT_SOUND_VOLUME);
        driftSoundLoaded_ = true;
    }

    std::cout << "Audio loaded successfully" << std::endl;
    return true;
}

void AudioManager::update(const Vehicle& vehicle) {
    if (!initialized_ || !soundLoaded_) {
        return;
    }

    const float absoluteVelocity = std::abs(vehicle.getVelocity());
    const bool nitrousActive = vehicle.isNitrousActive();
    const bool isDrifting = vehicle.isDrifting();

    // Update pitch based on speed (simulates engine RPM)
    const float referenceSpeed = nitrousActive ? NITROUS_REFERENCE_SPEED : BASE_REFERENCE_SPEED;
    float pitch = calculateEnginePitch(absoluteVelocity, referenceSpeed);

    // Add extra pitch boost during nitrous
    if (nitrousActive) {
        pitch *= NITROUS_PITCH_MULTIPLIER;
    }

    ma_sound_set_pitch(engineSound_.get(), pitch);

    // Update volume based on speed
    const float baseVolume = ENGINE_IDLE_VOLUME + (absoluteVelocity / BASE_REFERENCE_SPEED) * (ENGINE_MAX_VOLUME - ENGINE_IDLE_VOLUME);

    // Boost volume significantly during nitrous
    float volume = baseVolume;
    if (nitrousActive) {
        volume += NITROUS_VOLUME_BOOST;
    }

    // Cap volume at maximum to prevent distortion
    volume = std::min(volume, MAX_VOLUME);

    ma_sound_set_volume(engineSound_.get(), volume);

    // Handle drift sound
    if (driftSoundLoaded_) {
        if (isDrifting && absoluteVelocity > DRIFT_MIN_SPEED) {
            // Start drift sound if not already playing
            if (ma_sound_is_playing(driftSound_.get()) == MA_FALSE) {
                ma_sound_start(driftSound_.get());
            }
            // Adjust volume based on speed - louder when drifting faster
            float driftVolume = DRIFT_SOUND_MIN_VOLUME +
                               (absoluteVelocity / BASE_REFERENCE_SPEED) * (DRIFT_SOUND_MAX_VOLUME - DRIFT_SOUND_MIN_VOLUME);
            driftVolume = std::min(driftVolume, DRIFT_SOUND_MAX_VOLUME);
            ma_sound_set_volume(driftSound_.get(), driftVolume);
        } else {
            // Stop drift sound when not drifting or moving too slow
            if (ma_sound_is_playing(driftSound_.get()) == MA_TRUE) {
                ma_sound_stop(driftSound_.get());
            }
        }
    }
}

float AudioManager::calculateEnginePitch(float velocity, float maxSpeed) const {
    float speedRatio = std::min(velocity / maxSpeed, 1.0f);

    // Square root creates realistic RPM curve
    return ENGINE_PITCH_MIN + (std::sqrt(speedRatio) * ENGINE_PITCH_RANGE);
}
