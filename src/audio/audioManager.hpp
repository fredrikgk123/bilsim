#pragma once

#include <string>

class Vehicle;

class AudioManager {
  public:
    AudioManager();
    ~AudioManager();

    bool initialize(const std::string& engineSoundPath);
    void update(const Vehicle& vehicle);

  private:
    void* engine_;
    void* engineSound_;

    bool initialized_;
    bool soundLoaded_;

    float calculateEnginePitch(float velocity, float maxSpeed) const;
};
