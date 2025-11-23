// AI Assistance: GitHub Copilot was used for miniaudio API integration,
// real-time pitch modulation calculations, and audio streaming setup.

#include "audio/audio_manager.hpp"
#include "core/interfaces/IVehicleState.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// Undefine Windows min/max macros that conflict with std::min/std::max
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif


namespace {
    // Audio volume constants
    constexpr float ENGINE_IDLE_VOLUME = 0.3f;
    constexpr float ENGINE_IDLE_PITCH = 0.8f;
    constexpr float ENGINE_MAX_VOLUME = 0.8f;
    constexpr float NITROUS_VOLUME_BOOST = 0.3f;
    constexpr float NITROUS_PITCH_MULTIPLIER = 1.2f;
    constexpr float MAX_VOLUME = 1.0f;

    constexpr float DRIFT_SOUND_VOLUME = 0.4f;
    constexpr float DRIFT_SOUND_MIN_VOLUME = 0.3f;
    constexpr float DRIFT_SOUND_MAX_VOLUME = 0.6f;
    constexpr float DRIFT_MIN_SPEED = 5.0f;

    // Audio pitch constants
    constexpr float ENGINE_PITCH_MIN = 0.8f;
    constexpr float ENGINE_PITCH_MAX = 2.0f;
    constexpr float ENGINE_PITCH_RANGE = ENGINE_PITCH_MAX - ENGINE_PITCH_MIN;

    // Reference speeds for audio calculation
    constexpr float BASE_REFERENCE_SPEED = 20.0f;

    // Asset path
    const std::string DRIFT_SOUND_PATH = "assets/tireScreech.wav";
}

// Implement custom deleters
void AudioManager::AudioDeleter::operator()(ma_engine* engine) const noexcept {
    if (engine) {
        ma_engine_uninit(engine);
        delete engine;
    }
}

void AudioManager::AudioDeleter::operator()(ma_sound* sound) const noexcept {
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

bool AudioManager::initialize(std::string_view engineSoundPath) {
    // Initialize audio engine - use unique_ptr from the start for exception safety
    std::unique_ptr<ma_engine, AudioDeleter> engine(new ma_engine());
    ma_result result = ma_engine_init(nullptr, engine.get());

    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine: " << result << std::endl;
        return false;
    }
    engine_ = std::move(engine);
    initialized_ = true;

    // Load engine sound file - use unique_ptr from the start
    std::unique_ptr<ma_sound, AudioDeleter> sound(new ma_sound());
    result = ma_sound_init_from_file(engine_.get(), engineSoundPath.data(),
                                     MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION,
                                     nullptr, nullptr, sound.get());

    if (result != MA_SUCCESS) {
        std::cerr << "Failed to load engine sound from: " << engineSoundPath << " (error: " << result << ")" << std::endl;
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
        std::cerr << "Warning: Failed to load drift sound from: " << DRIFT_SOUND_PATH << " (error: " << result << ")" << std::endl;
        return false;

    } else {
        driftSound_ = std::move(driftSnd);
        ma_sound_set_looping(driftSound_.get(), MA_TRUE);
        ma_sound_set_volume(driftSound_.get(), DRIFT_SOUND_VOLUME);
        driftSoundLoaded_ = true;
    }

    return true;
}

void AudioManager::update(const IVehicleState& vehicleState) {
    if (!initialized_ || !soundLoaded_) {
        return;
    }

    const float rpm = vehicleState.getRPM();
    const int currentGear = vehicleState.getCurrentGear();
    const bool nitrousActive = vehicleState.isNitrousActive();
    const bool isDrifting = vehicleState.isDrifting();
    const float absoluteVelocity = (std::abs)(vehicleState.getVelocity());

    // Calculate pitch based on RPM
    constexpr float MIN_RPM = 1000.0f;
    constexpr float MAX_RPM = 7000.0f;
    float rpmRatio = (rpm - MIN_RPM) / (MAX_RPM - MIN_RPM);
    rpmRatio = (std::clamp)(rpmRatio, 0.0f, 1.0f);

    float pitch = ENGINE_PITCH_MIN + (rpmRatio * ENGINE_PITCH_RANGE);

    if (nitrousActive) {
        pitch *= NITROUS_PITCH_MULTIPLIER;
    }

    ma_sound_set_pitch(engineSound_.get(), pitch);

    // Update volume based on RPM
    float baseVolume = ENGINE_IDLE_VOLUME + (rpmRatio * (ENGINE_MAX_VOLUME - ENGINE_IDLE_VOLUME));

    // Volume reduction for lower gears
    float gearVolumeMultiplier = 1.0f;
    if (currentGear == 1) {
        gearVolumeMultiplier = 0.6f;
    } else if (currentGear == 2) {
        gearVolumeMultiplier = 0.75f;
    } else if (currentGear == 3) {
        gearVolumeMultiplier = 0.9f;
    }

    float volume = baseVolume * gearVolumeMultiplier;

    if (nitrousActive) {
        volume += NITROUS_VOLUME_BOOST;
    }

    volume = (std::min)(volume, MAX_VOLUME);

    ma_sound_set_volume(engineSound_.get(), volume);

    // Handle drift sound
    if (driftSoundLoaded_) {
        if (isDrifting && absoluteVelocity > DRIFT_MIN_SPEED) {
            if (ma_sound_is_playing(driftSound_.get()) == MA_FALSE) {
                ma_sound_start(driftSound_.get());
            }
            float driftVolume = DRIFT_SOUND_MIN_VOLUME +
                               (absoluteVelocity / BASE_REFERENCE_SPEED) * (DRIFT_SOUND_MAX_VOLUME - DRIFT_SOUND_MIN_VOLUME);
            driftVolume = (std::min)(driftVolume, DRIFT_SOUND_MAX_VOLUME);
            ma_sound_set_volume(driftSound_.get(), driftVolume);
        } else {
            if (ma_sound_is_playing(driftSound_.get()) == MA_TRUE) {
                ma_sound_stop(driftSound_.get());
            }
        }
    }
}

float AudioManager::calculateEnginePitch(float velocity, float maxSpeed) noexcept {
    float speedRatio = (std::min)(velocity / maxSpeed, 1.0f);

    // Square root creates realistic RPM curve
    return ENGINE_PITCH_MIN + ((std::sqrt)(speedRatio) * ENGINE_PITCH_RANGE);
}
