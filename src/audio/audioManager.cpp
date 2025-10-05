#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "audioManager.hpp"
#include "vehicle.hpp"
#include <iostream>
#include <cmath>

AudioManager::AudioManager()
    : engine_(nullptr),
      engineSound_(nullptr),
      initialized_(false),
      soundLoaded_(false) {
}

AudioManager::~AudioManager() {
    if (soundLoaded_ && engineSound_) {
        ma_sound_uninit((ma_sound*)engineSound_);
        delete (ma_sound*)engineSound_;
    }

    if (initialized_ && engine_) {
        ma_engine_uninit((ma_engine*)engine_);
        delete (ma_engine*)engine_;
    }
}

bool AudioManager::initialize(const std::string& engineSoundPath) {
    // Initialize audio engine
    engine_ = new ma_engine();
    if (ma_engine_init(nullptr, (ma_engine*)engine_) != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine" << std::endl;
        delete (ma_engine*)engine_;
        engine_ = nullptr;
        return false;
    }
    initialized_ = true;

    // Load engine sound file
    engineSound_ = new ma_sound();
    if (ma_sound_init_from_file((ma_engine*)engine_, engineSoundPath.c_str(),
                                 MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION,
                                 nullptr, nullptr, (ma_sound*)engineSound_) != MA_SUCCESS) {
        std::cout << "Engine sound not found at: " << engineSoundPath << std::endl;
        delete (ma_sound*)engineSound_;
        engineSound_ = nullptr;
        return false;
    }

    // Configure sound playback
    ma_sound_set_looping((ma_sound*)engineSound_, MA_TRUE);
    ma_sound_set_volume((ma_sound*)engineSound_, 0.3f);  // 0.3 = 30% - idle engine volume (quiet)
    ma_sound_set_pitch((ma_sound*)engineSound_, 0.8f);   // 0.8 = 80% speed - lower pitch for idle
    ma_sound_start((ma_sound*)engineSound_);

    soundLoaded_ = true;
    std::cout << "Audio loaded successfully" << std::endl;
    return true;
}

void AudioManager::update(const Vehicle& vehicle) {
    if (!initialized_ || !soundLoaded_) {
        return;
    }

    float absVelocity = std::abs(vehicle.getVelocity());

    // Update pitch based on speed (simulates engine RPM)
    float pitch = calculateEnginePitch(absVelocity, 20.0f);  // 20.0 reference speed - tuned for good audio response
    ma_sound_set_pitch((ma_sound*)engineSound_, pitch);

    // Update volume based on speed
    float volume = 0.3f + (absVelocity / 20.0f) * 0.5f;  // Range: 0.3 (idle) to 0.8 (full throttle)
    if (volume > 0.8f) {  // 0.8 cap - prevents audio distortion
        volume = 0.8f;
    }
    ma_sound_set_volume((ma_sound*)engineSound_, volume);
}

float AudioManager::calculateEnginePitch(float velocity, float maxSpeed) const {
    float speedRatio = velocity / maxSpeed;
    if (speedRatio > 1.0f) {
        speedRatio = 1.0f;
    }

    // Pitch range: 0.8 (idle) to 2.0 (max RPM)
    return 0.8f + (std::sqrt(speedRatio) * 1.2f);  // 1.2 range (2.0 - 0.8) - square root creates realistic RPM curve
}
