#pragma once

#include <threepp/threepp.hpp>
#include "vehicle.hpp"
#include <memory>
#include <vector>

class UIManager {
public:
    UIManager(threepp::GLRenderer& renderer);

    // Render UI overlay
    void render(const Vehicle& vehicle, const threepp::WindowSize& size);

private:
    void createSpeedometerGeometry();
    void updateSpeedometer(float speed);
    void createSevenSegmentDigit(float xPos, float yPos, int digitIndex);

    threepp::GLRenderer& renderer_;

    // HUD scene and camera
    std::shared_ptr<threepp::Scene> hudScene_;
    std::shared_ptr<threepp::OrthographicCamera> hudCamera_;

    // Speedometer components
    std::shared_ptr<threepp::Mesh> speedometerBackground_;
    std::shared_ptr<threepp::Mesh> speedometerFill_;
    std::shared_ptr<threepp::Mesh> speedometerOutline_;

    // Seven-segment display digits
    std::vector<std::vector<std::shared_ptr<threepp::Mesh>>> digitSegments_;

    // Nitrous display components
    std::shared_ptr<threepp::Mesh> nitrousIndicator_;
    std::shared_ptr<threepp::Mesh> nitrousBar_;
    std::shared_ptr<threepp::Mesh> nitrousBarOutline_;
};
