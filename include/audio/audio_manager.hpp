// AI Assistance: GitHub Copilot was used for miniaudio API integration
// and custom deleters for C resource management.

#pragma once

#include <string>
#include <string_view>
#include <memory>

// Forward declare miniaudio types
struct ma_engine;
struct ma_sound;

class IVehicleState;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    // Delete copy constructor and assignment operator (audio resources shouldn't be copied)
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // Initialize audio engine and load sound files
    [[nodiscard]] bool initialize(std::string_view engineSoundPath);

    // Update audio based on vehicle state
    void update(const IVehicleState& vehicleState);

private:
    [[nodiscard]] static float calculateEnginePitch(float velocity, float maxSpeed) noexcept;

    struct AudioDeleter {
        void operator()(ma_engine* engine) const noexcept;
        void operator()(ma_sound* sound) const noexcept;
    };

    std::unique_ptr<ma_engine, AudioDeleter> engine_;
    std::unique_ptr<ma_sound, AudioDeleter> engineSound_;
    std::unique_ptr<ma_sound, AudioDeleter> driftSound_;
    bool initialized_;
    bool soundLoaded_;
    bool driftSoundLoaded_;
};
