#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "audioManager.hpp"
#include "../core/vehicle.hpp"
#include <iostream>
#include <cmath>

// Implement custom deleters
void AudioManager::AudioDeleter::operator()(ma_engine* engine) const {
    if (engine != nullptr) {
        ma_engine_uninit(engine);
        delete engine;
    }
}

void AudioManager::AudioDeleter::operator()(ma_sound* sound) const {
    if (sound != nullptr) {
        ma_sound_uninit(sound);
        delete sound;
    }
}

AudioManager::AudioManager()
    : engine_(nullptr),
      engineSound_(nullptr),
      initialized_(false),
      soundLoaded_(false) {
}

AudioManager::~AudioManager() = default;  // unique_ptr handles cleanup automatically

bool AudioManager::initialize(const std::string& engineSoundPath) {
    // Initialize audio engine
    ma_engine* engine = new ma_engine();
    ma_result result = ma_engine_init(nullptr, engine);

    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine" << std::endl;
        delete engine;
        return false;
    }
    engine_.reset(engine);
    initialized_ = true;

    // Load engine sound file
    ma_sound* sound = new ma_sound();
    result = ma_sound_init_from_file(engine_.get(), engineSoundPath.c_str(),
                                     MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION,
                                     nullptr, nullptr, sound);

    if (result != MA_SUCCESS) {
        std::cout << "Engine sound not found at: " << engineSoundPath << std::endl;
        delete sound;
        return false;
    }
    engineSound_.reset(sound);

    // Configure sound playback
    ma_sound_set_looping(engineSound_.get(), MA_TRUE);
    ma_sound_set_volume(engineSound_.get(), 0.3f);  // 0.3 = 30% - idle engine volume (quiet)
    ma_sound_set_pitch(engineSound_.get(), 0.8f);   // 0.8 = 80% speed - lower pitch for idle
    ma_sound_start(engineSound_.get());

    soundLoaded_ = true;
    std::cout << "Audio loaded successfully" << std::endl;
    return true;
}

void AudioManager::update(const Vehicle& vehicle) {
    if (initialized_ == false || soundLoaded_ == false) {
        return;
    }

    float absVelocity = std::abs(vehicle.getVelocity());
    bool nitrousActive = vehicle.isNitrousActive();

    // Update pitch based on speed (simulates engine RPM)
    // Boost pitch and reference speed during nitrous for more aggressive sound
    float referenceSpeed = 20.0f;
    if (nitrousActive == true) {
        referenceSpeed = 25.0f;  // Higher reference speed during nitrous
    }

    float pitch = calculateEnginePitch(absVelocity, referenceSpeed);

    // Add extra pitch boost during nitrous
    if (nitrousActive == true) {
        pitch = pitch * 1.2f;  // 20% higher pitch during nitrous
    }

    ma_sound_set_pitch(engineSound_.get(), pitch);

    // Update volume based on speed
    float baseVolume = 0.3f + (absVelocity / 20.0f) * 0.5f;  // Range: 0.3 (idle) to 0.8 (full throttle)

    // Boost volume significantly during nitrous
    float volume = baseVolume;
    if (nitrousActive == true) {
        volume = volume + 0.3f;  // Add 30% more volume during nitrous
    }

    // Cap volume at 1.0 to prevent distortion
    if (volume > 1.0f) {
        volume = 1.0f;
    }

    ma_sound_set_volume(engineSound_.get(), volume);
}

float AudioManager::calculateEnginePitch(float velocity, float maxSpeed) const {
    float speedRatio = velocity / maxSpeed;

    // Clamp speed ratio to 1.0
    if (speedRatio > 1.0f) {
        speedRatio = 1.0f;
    }

    // Pitch range: 0.8 (idle) to 2.0 (max RPM)
    float pitch = 0.8f + (std::sqrt(speedRatio) * 1.2f);  // 1.2 range (2.0 - 0.8) - square root creates realistic RPM curve
    return pitch;
}
