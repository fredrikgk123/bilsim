#include "uiManager.hpp"
#include "../core/vehicle.hpp"
#include <cmath>
#include <array>

using namespace threepp;

// Anonymous namespace - these constants are LOCAL to this file only (NOT global!)
namespace {
    // Seven-segment display patterns for digits 0-9
    // Segments: top, top-right, bottom-right, bottom, bottom-left, top-left, middle
    const std::array<std::array<bool, 7>, 10> SEGMENT_PATTERNS = {{
        {true, true, true, true, true, true, false},      // 0
        {false, true, true, false, false, false, false},  // 1
        {true, true, false, true, true, false, true},     // 2
        {true, true, true, true, false, false, true},     // 3
        {false, true, true, false, false, true, true},    // 4
        {true, false, true, true, false, true, true},     // 5
        {true, false, true, true, true, true, true},      // 6
        {true, true, true, false, false, false, false},   // 7
        {true, true, true, true, true, true, true},       // 8
        {true, true, true, true, false, true, true}       // 9
    }};
}

UIManager::UIManager(GLRenderer& renderer)
    : renderer_(renderer) {
    hudScene_ = std::make_shared<Scene>();

    // Orthographic camera for 2D overlay
    hudCamera_ = std::make_shared<OrthographicCamera>(-1, 1, 1, -1, 0.1f, 10.0f);  // -1 to 1 = normalized screen space
    hudCamera_->position.z = 1;

    createSpeedometerGeometry();

    // Create nitrous indicator
    auto nitrousGeometry = PlaneGeometry::create(0.08f, 0.08f);
    auto nitrousMaterial = MeshBasicMaterial::create();
    nitrousMaterial->color = Color(0x00aaff); // Bright blue
    nitrousMaterial->transparent = true;
    nitrousMaterial->opacity = 0.9f;
    nitrousMaterial->depthTest = false;

    nitrousIndicator_ = Mesh::create(nitrousGeometry, nitrousMaterial);
    nitrousIndicator_->position.set(0.6f, -0.85f, 0.03f);
    nitrousIndicator_->visible = false;
    hudScene_->add(nitrousIndicator_);

    // Create nitrous bar outline
    auto nitrousOutlineGeometry = PlaneGeometry::create(0.32f, 0.03f);
    auto nitrousOutlineMaterial = MeshBasicMaterial::create();
    nitrousOutlineMaterial->color = Color(0x333333);
    nitrousOutlineMaterial->transparent = true;
    nitrousOutlineMaterial->opacity = 0.8f;
    nitrousOutlineMaterial->depthTest = false;

    nitrousBarOutline_ = Mesh::create(nitrousOutlineGeometry, nitrousOutlineMaterial);
    nitrousBarOutline_->position.set(0.6f, -0.85f, 0.01f);
    nitrousBarOutline_->visible = false;
    hudScene_->add(nitrousBarOutline_);

    // Create nitrous bar fill
    auto nitrousBarGeometry = PlaneGeometry::create(0.32f, 0.03f);
    auto nitrousBarMaterial = MeshBasicMaterial::create();
    nitrousBarMaterial->color = Color(0x00aaff);
    nitrousBarMaterial->transparent = true;
    nitrousBarMaterial->opacity = 0.9f;
    nitrousBarMaterial->depthTest = false;

    nitrousBar_ = Mesh::create(nitrousBarGeometry, nitrousBarMaterial);
    nitrousBar_->position.set(0.6f, -0.85f, 0.02f);
    nitrousBar_->visible = false;
    hudScene_->add(nitrousBar_);
}

void UIManager::createSpeedometerGeometry() {
    // Create background panel
    auto bgGeometry = PlaneGeometry::create(0.4f, 0.25f);  // 0.4x0.25 units - sized to fit speedometer components
    auto bgMaterial = MeshBasicMaterial::create();
    bgMaterial->color = Color(0x000000);  // Black background
    bgMaterial->transparent = true;
    bgMaterial->opacity = 0.7f;  // 70% opacity - semi-transparent so scene shows through
    bgMaterial->depthTest = false;

    speedometerBackground_ = Mesh::create(bgGeometry, bgMaterial);
    speedometerBackground_->position.set(0.6f, -0.725f, 0);  // (0.6, -0.725) = bottom-right corner in normalized space
    hudScene_->add(speedometerBackground_);

    // Create bar outline
    auto outlineGeometry = PlaneGeometry::create(0.32f, 0.04f);  // 0.32x0.04 - horizontal bar shape
    auto outlineMaterial = MeshBasicMaterial::create();
    outlineMaterial->color = Color(0x333333);  // Dark gray
    outlineMaterial->transparent = true;
    outlineMaterial->opacity = 0.8f;  // 80% opacity - slightly visible
    outlineMaterial->depthTest = false;

    speedometerOutline_ = Mesh::create(outlineGeometry, outlineMaterial);
    speedometerOutline_->position.set(0.6f, -0.68f, 0.01f);  // z=0.01 - behind fill bar
    hudScene_->add(speedometerOutline_);

    // Create fill bar
    auto fillGeometry = PlaneGeometry::create(0.32f, 0.04f);
    auto fillMaterial = MeshBasicMaterial::create();
    fillMaterial->color = Color(0x00ff00);  // Bright green
    fillMaterial->transparent = true;
    fillMaterial->opacity = 0.9f;  // 90% opacity - highly visible
    fillMaterial->depthTest = false;

    speedometerFill_ = Mesh::create(fillGeometry, fillMaterial);
    speedometerFill_->position.set(0.6f, -0.68f, 0.02f);  // z=0.02 - in front of outline
    speedometerFill_->scale.x = 0.0f;  // Start empty (0 speed)
    hudScene_->add(speedometerFill_);

    // Create three digits for speed display
    digitSegments_.resize(3);  // 3 digits - allows 0-999 display range

    float digitStartX = 0.52f;      // Start position - left of speedometer center
    float digitSpacing = 0.055f;    // 0.055 units between digits - tight spacing for compact display
    float digitY = -0.775f;         // Below the bar

    for (int i = 0; i < 3; i = i + 1) {
        float xPosition = digitStartX + (i * digitSpacing);
        createSevenSegmentDigit(xPosition, digitY, i);
    }
}

void UIManager::createSevenSegmentDigit(float xPos, float yPos, int digitIndex) {
    auto segmentMaterial = MeshBasicMaterial::create();
    segmentMaterial->color = Color(0x00ff00);  // Green - matches speedometer bar
    segmentMaterial->transparent = true;
    segmentMaterial->opacity = 0.9f;  // 90% opacity - highly visible
    segmentMaterial->depthTest = false;

    std::vector<std::shared_ptr<Mesh>> segments;

    float segmentHeight = 0.006f;   // 0.006 units - thin segment thickness
    float segmentLength = 0.025f;   // 0.025 units - segment length for readable digit

    auto hSegmentGeometry = PlaneGeometry::create(segmentLength, segmentHeight);  // Horizontal segments
    auto vSegmentGeometry = PlaneGeometry::create(segmentHeight, segmentLength);  // Vertical segments (rotated)

    // Create 7 segments for this digit
    // Segment 0: Top horizontal
    auto seg0 = Mesh::create(hSegmentGeometry, segmentMaterial);
    seg0->position.set(xPos, yPos + 0.028f, 0.03f);  // +0.028 = top position
    hudScene_->add(seg0);
    segments.push_back(seg0);

    // Segment 1: Top right vertical
    auto seg1 = Mesh::create(vSegmentGeometry, segmentMaterial);
    seg1->position.set(xPos + 0.0135f, yPos + 0.014f, 0.03f);  // +0.0135 right, +0.014 upper half
    hudScene_->add(seg1);
    segments.push_back(seg1);

    // Segment 2: Bottom right vertical
    auto seg2 = Mesh::create(vSegmentGeometry, segmentMaterial);
    seg2->position.set(xPos + 0.0135f, yPos - 0.014f, 0.03f);  // +0.0135 right, -0.014 lower half
    hudScene_->add(seg2);
    segments.push_back(seg2);

    // Segment 3: Bottom horizontal
    auto seg3 = Mesh::create(hSegmentGeometry, segmentMaterial);
    seg3->position.set(xPos, yPos - 0.028f, 0.03f);  // -0.028 = bottom position
    hudScene_->add(seg3);
    segments.push_back(seg3);

    // Segment 4: Bottom left vertical
    auto seg4 = Mesh::create(vSegmentGeometry, segmentMaterial);
    seg4->position.set(xPos - 0.0135f, yPos - 0.014f, 0.03f);  // -0.0135 left, -0.014 lower half
    hudScene_->add(seg4);
    segments.push_back(seg4);

    // Segment 5: Top left vertical
    auto seg5 = Mesh::create(vSegmentGeometry, segmentMaterial);
    seg5->position.set(xPos - 0.0135f, yPos + 0.014f, 0.03f);  // -0.0135 left, +0.014 upper half
    hudScene_->add(seg5);
    segments.push_back(seg5);

    // Segment 6: Middle horizontal
    auto seg6 = Mesh::create(hSegmentGeometry, segmentMaterial);
    seg6->position.set(xPos, yPos, 0.03f);  // Center position
    hudScene_->add(seg6);
    segments.push_back(seg6);

    digitSegments_[digitIndex] = segments;

    // Initially hide all segments
    for (int s = 0; s < segments.size(); s = s + 1) {
        segments[s]->visible = false;
    }
}

void UIManager::updateSpeedometer(float speed) {
    // Calculate speed display
    float maxSpeed = 150.0f;  // 150 km/h - matches vehicle's real max speed
    float displaySpeed = std::abs(speed) * 3.6f;  // 1 unit/sec = 3.6 km/h
    float speedRatio = displaySpeed / maxSpeed;

    // Clamp to 0-1 range
    if (speedRatio > 1.0f) {
        speedRatio = 1.0f;
    }

    // Update fill bar scale
    speedometerFill_->scale.x = speedRatio;

    // Position bar to grow from left to right
    float barWidth = 0.32f;  // Same as geometry width
    speedometerFill_->position.x = 0.6f - ((barWidth / 2.0f) * (1.0f - speedRatio));  // Offset calculation for left-to-right growth

    // Update seven-segment display
    int speedInt = static_cast<int>(displaySpeed);
    int hundreds = (speedInt / 100) % 10;
    int tens = (speedInt / 10) % 10;
    int ones = speedInt % 10;

    std::array<int, 3> digits = {hundreds, tens, ones};

    for (int d = 0; d < 3; d = d + 1) {
        int digit = digits[d];

        // Hide leading zeros
        bool hideDigit = false;
        if (d == 0 && speedInt < 100) {
            hideDigit = true;
        }
        if (d == 1 && speedInt < 10) {
            hideDigit = true;
        }

        if (hideDigit == true) {
            for (int s = 0; s < 7; s = s + 1) {
                digitSegments_[d][s]->visible = false;
            }
        } else {
            // Show segments based on digit pattern
            for (int s = 0; s < 7; s = s + 1) {
                digitSegments_[d][s]->visible = SEGMENT_PATTERNS[digit][s];
            }
        }
    }
}

void UIManager::render(const Vehicle& vehicle, const WindowSize& size) {
    float speed = vehicle.getVelocity();
    updateSpeedometer(speed);

    // Update nitrous indicator
    if (vehicle.hasNitrous() == true) {
        nitrousIndicator_->visible = true;
        nitrousBarOutline_->visible = false;
        nitrousBar_->visible = false;
    } else if (vehicle.isNitrousActive() == true) {
        nitrousIndicator_->visible = false;
        nitrousBarOutline_->visible = true;
        nitrousBar_->visible = true;

        // Update nitrous bar based on time remaining
        float timeRemaining = vehicle.getNitrousTimeRemaining();
        float nitrousRatio = timeRemaining / 5.0f; // 5 seconds duration

        nitrousBar_->scale.x = nitrousRatio;

        float barWidth = 0.32f;
        nitrousBar_->position.x = 0.6f - ((barWidth / 2.0f) * (1.0f - nitrousRatio));
    } else {
        nitrousIndicator_->visible = false;
        nitrousBarOutline_->visible = false;
        nitrousBar_->visible = false;
    }

    // Render HUD overlay on top of main scene
    renderer_.autoClear = false;
    renderer_.render(*hudScene_, *hudCamera_);
    renderer_.autoClear = true;
}
