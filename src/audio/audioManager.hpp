#pragma once

#include <string>

class Vehicle;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    // Initialize audio engine and load sound file
    bool initialize(const std::string& engineSoundPath);

    // Update audio based on vehicle state
    void update(const Vehicle& vehicle);

private:
    float calculateEnginePitch(float velocity, float maxSpeed) const;

    void* engine_;          // miniaudio engine
    void* engineSound_;     // miniaudio sound
    bool initialized_;
    bool soundLoaded_;
};
