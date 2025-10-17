#include "uiManager.hpp"
#include "../core/vehicle.hpp"
#include <cmath>
#include <array>

using namespace threepp;

// Seven-segment display implementation inspired by standard digital display logic
// Pattern encoding based on common 7-segment layout conventions
// Reference: https://en.wikipedia.org/wiki/Seven-segment_display
//
// Segment layout:
//     [0]
//   [5] [1]
//     [6]
//   [4] [2]
//     [3]

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

    // Speedometer dimensions and positioning
    constexpr float SPEEDOMETER_BG_WIDTH = 0.4f;
    constexpr float SPEEDOMETER_BG_HEIGHT = 0.25f;
    constexpr float SPEEDOMETER_BAR_WIDTH = 0.32f;
    constexpr float SPEEDOMETER_BAR_HEIGHT = 0.04f;
    constexpr float SPEEDOMETER_X_POS = 0.6f;          // Right side of screen
    constexpr float SPEEDOMETER_BG_Y_POS = -0.725f;
    constexpr float SPEEDOMETER_BAR_Y_POS = -0.68f;
    constexpr float SPEEDOMETER_DIGIT_Y_POS = -0.775f;
    constexpr float SPEEDOMETER_DIGIT_START_X = 0.52f;
    constexpr float SPEEDOMETER_DIGIT_SPACING = 0.055f;

    // Seven-segment display dimensions
    constexpr float SEGMENT_HEIGHT = 0.006f;
    constexpr float SEGMENT_LENGTH = 0.025f;
    constexpr float SEGMENT_VERTICAL_OFFSET = 0.028f;
    constexpr float SEGMENT_HORIZONTAL_OFFSET = 0.0135f;
    constexpr float SEGMENT_HALF_OFFSET = 0.014f;
    constexpr int NUM_DIGITS = 3;
    constexpr int NUM_SEGMENTS_PER_DIGIT = 7;

    // Nitrous indicator positioning
    constexpr float NITROUS_INDICATOR_SIZE = 0.08f;
    constexpr float NITROUS_BAR_WIDTH = 0.32f;
    constexpr float NITROUS_BAR_HEIGHT = 0.03f;
    constexpr float NITROUS_X_POS = 0.6f;
    constexpr float NITROUS_Y_POS = -0.85f;

    // Material properties
    constexpr float BACKGROUND_OPACITY = 0.7f;
    constexpr float OUTLINE_OPACITY = 0.8f;
    constexpr float FILL_OPACITY = 0.9f;
    constexpr float SEGMENT_OPACITY = 0.9f;

    // Colors
    constexpr unsigned int BLACK_COLOR = 0x000000;
    constexpr unsigned int DARK_GRAY_COLOR = 0x333333;
    constexpr unsigned int GREEN_COLOR = 0x00ff00;
    constexpr unsigned int NITROUS_BLUE_COLOR = 0x00aaff;

    // Speed constants
    constexpr float MAX_DISPLAY_SPEED_KMH = 150.0f;  // 150 km/h - matches vehicle's real max speed
    constexpr float MS_TO_KMH = 3.6f;                // Conversion: 1 m/s = 3.6 km/h
    constexpr float NITROUS_DURATION = 5.0f;         // 5 seconds - must match vehicle.cpp

    // Z-depth layers
    constexpr float Z_BACKGROUND = 0.0f;
    constexpr float Z_OUTLINE = 0.01f;
    constexpr float Z_FILL = 0.02f;
    constexpr float Z_SEGMENTS = 0.03f;
}

UIManager::UIManager(GLRenderer& renderer)
    : renderer_(renderer) {
    hudScene_ = std::make_shared<Scene>();

    // Orthographic camera for 2D overlay
    hudCamera_ = std::make_shared<OrthographicCamera>(-1, 1, 1, -1, 0.1f, 10.0f);
    hudCamera_->position.z = 1;

    createSpeedometerGeometry();

    // Create nitrous indicator
    auto nitrousGeometry = PlaneGeometry::create(NITROUS_INDICATOR_SIZE, NITROUS_INDICATOR_SIZE);
    auto nitrousMaterial = MeshBasicMaterial::create();
    nitrousMaterial->color = Color(NITROUS_BLUE_COLOR);
    nitrousMaterial->transparent = true;
    nitrousMaterial->opacity = FILL_OPACITY;
    nitrousMaterial->depthTest = false;

    nitrousIndicator_ = Mesh::create(nitrousGeometry, nitrousMaterial);
    nitrousIndicator_->position.set(NITROUS_X_POS, NITROUS_Y_POS, Z_SEGMENTS);
    nitrousIndicator_->visible = false;
    hudScene_->add(nitrousIndicator_);

    // Create nitrous bar outline
    auto nitrousOutlineGeometry = PlaneGeometry::create(NITROUS_BAR_WIDTH, NITROUS_BAR_HEIGHT);
    auto nitrousOutlineMaterial = MeshBasicMaterial::create();
    nitrousOutlineMaterial->color = Color(DARK_GRAY_COLOR);
    nitrousOutlineMaterial->transparent = true;
    nitrousOutlineMaterial->opacity = OUTLINE_OPACITY;
    nitrousOutlineMaterial->depthTest = false;

    nitrousBarOutline_ = Mesh::create(nitrousOutlineGeometry, nitrousOutlineMaterial);
    nitrousBarOutline_->position.set(NITROUS_X_POS, NITROUS_Y_POS, Z_OUTLINE);
    nitrousBarOutline_->visible = false;
    hudScene_->add(nitrousBarOutline_);

    // Create nitrous bar fill
    auto nitrousBarGeometry = PlaneGeometry::create(NITROUS_BAR_WIDTH, NITROUS_BAR_HEIGHT);
    auto nitrousBarMaterial = MeshBasicMaterial::create();
    nitrousBarMaterial->color = Color(NITROUS_BLUE_COLOR);
    nitrousBarMaterial->transparent = true;
    nitrousBarMaterial->opacity = FILL_OPACITY;
    nitrousBarMaterial->depthTest = false;

    nitrousBar_ = Mesh::create(nitrousBarGeometry, nitrousBarMaterial);
    nitrousBar_->position.set(NITROUS_X_POS, NITROUS_Y_POS, Z_FILL);
    nitrousBar_->visible = false;
    hudScene_->add(nitrousBar_);
}

void UIManager::createSpeedometerGeometry() {
    // Create background panel
    auto bgGeometry = PlaneGeometry::create(SPEEDOMETER_BG_WIDTH, SPEEDOMETER_BG_HEIGHT);
    auto bgMaterial = MeshBasicMaterial::create();
    bgMaterial->color = Color(BLACK_COLOR);
    bgMaterial->transparent = true;
    bgMaterial->opacity = BACKGROUND_OPACITY;
    bgMaterial->depthTest = false;

    speedometerBackground_ = Mesh::create(bgGeometry, bgMaterial);
    speedometerBackground_->position.set(SPEEDOMETER_X_POS, SPEEDOMETER_BG_Y_POS, Z_BACKGROUND);
    hudScene_->add(speedometerBackground_);

    // Create bar outline
    auto outlineGeometry = PlaneGeometry::create(SPEEDOMETER_BAR_WIDTH, SPEEDOMETER_BAR_HEIGHT);
    auto outlineMaterial = MeshBasicMaterial::create();
    outlineMaterial->color = Color(DARK_GRAY_COLOR);
    outlineMaterial->transparent = true;
    outlineMaterial->opacity = OUTLINE_OPACITY;
    outlineMaterial->depthTest = false;

    speedometerOutline_ = Mesh::create(outlineGeometry, outlineMaterial);
    speedometerOutline_->position.set(SPEEDOMETER_X_POS, SPEEDOMETER_BAR_Y_POS, Z_OUTLINE);
    hudScene_->add(speedometerOutline_);

    // Create fill bar
    auto fillGeometry = PlaneGeometry::create(SPEEDOMETER_BAR_WIDTH, SPEEDOMETER_BAR_HEIGHT);
    auto fillMaterial = MeshBasicMaterial::create();
    fillMaterial->color = Color(GREEN_COLOR);
    fillMaterial->transparent = true;
    fillMaterial->opacity = FILL_OPACITY;
    fillMaterial->depthTest = false;

    speedometerFill_ = Mesh::create(fillGeometry, fillMaterial);
    speedometerFill_->position.set(SPEEDOMETER_X_POS, SPEEDOMETER_BAR_Y_POS, Z_FILL);
    speedometerFill_->scale.x = 0.0f;  // Start empty (0 speed)
    hudScene_->add(speedometerFill_);

    // Create three digits for speed display
    digitSegments_.resize(NUM_DIGITS);

    for (int i = 0; i < NUM_DIGITS; ++i) {
        float xPosition = SPEEDOMETER_DIGIT_START_X + (i * SPEEDOMETER_DIGIT_SPACING);
        createSevenSegmentDigit(xPosition, SPEEDOMETER_DIGIT_Y_POS, i);
    }
}

void UIManager::createSevenSegmentDigit(float xPos, float yPos, int digitIndex) {
    auto segmentMaterial = MeshBasicMaterial::create();
    segmentMaterial->color = Color(GREEN_COLOR);
    segmentMaterial->transparent = true;
    segmentMaterial->opacity = SEGMENT_OPACITY;
    segmentMaterial->depthTest = false;

    std::vector<std::shared_ptr<Mesh>> segments;

    auto hSegmentGeometry = PlaneGeometry::create(SEGMENT_LENGTH, SEGMENT_HEIGHT);  // Horizontal segments
    auto vSegmentGeometry = PlaneGeometry::create(SEGMENT_HEIGHT, SEGMENT_LENGTH);  // Vertical segments

    // Create 7 segments for this digit
    // Segment 0: Top horizontal
    auto seg0 = Mesh::create(hSegmentGeometry, segmentMaterial);
    seg0->position.set(xPos, yPos + SEGMENT_VERTICAL_OFFSET, Z_SEGMENTS);
    hudScene_->add(seg0);
    segments.push_back(seg0);

    // Segment 1: Top right vertical
    auto seg1 = Mesh::create(vSegmentGeometry, segmentMaterial);
    seg1->position.set(xPos + SEGMENT_HORIZONTAL_OFFSET, yPos + SEGMENT_HALF_OFFSET, Z_SEGMENTS);
    hudScene_->add(seg1);
    segments.push_back(seg1);

    // Segment 2: Bottom right vertical
    auto seg2 = Mesh::create(vSegmentGeometry, segmentMaterial);
    seg2->position.set(xPos + SEGMENT_HORIZONTAL_OFFSET, yPos - SEGMENT_HALF_OFFSET, Z_SEGMENTS);
    hudScene_->add(seg2);
    segments.push_back(seg2);

    // Segment 3: Bottom horizontal
    auto seg3 = Mesh::create(hSegmentGeometry, segmentMaterial);
    seg3->position.set(xPos, yPos - SEGMENT_VERTICAL_OFFSET, Z_SEGMENTS);
    hudScene_->add(seg3);
    segments.push_back(seg3);

    // Segment 4: Bottom left vertical
    auto seg4 = Mesh::create(vSegmentGeometry, segmentMaterial);
    seg4->position.set(xPos - SEGMENT_HORIZONTAL_OFFSET, yPos - SEGMENT_HALF_OFFSET, Z_SEGMENTS);
    hudScene_->add(seg4);
    segments.push_back(seg4);

    // Segment 5: Top left vertical
    auto seg5 = Mesh::create(vSegmentGeometry, segmentMaterial);
    seg5->position.set(xPos - SEGMENT_HORIZONTAL_OFFSET, yPos + SEGMENT_HALF_OFFSET, Z_SEGMENTS);
    hudScene_->add(seg5);
    segments.push_back(seg5);

    // Segment 6: Middle horizontal
    auto seg6 = Mesh::create(hSegmentGeometry, segmentMaterial);
    seg6->position.set(xPos, yPos, Z_SEGMENTS);
    hudScene_->add(seg6);
    segments.push_back(seg6);

    digitSegments_[digitIndex] = segments;

    // Initially hide all segments
    for (auto& segment : segments) {
        segment->visible = false;
    }
}

void UIManager::updateSpeedometer(float speed) {
    // Calculate speed display
    float displaySpeed = std::abs(speed) * MS_TO_KMH;
    float speedRatio = std::clamp(displaySpeed / MAX_DISPLAY_SPEED_KMH, 0.0f, 1.0f);

    // Update fill bar scale
    speedometerFill_->scale.x = speedRatio;

    // Position bar to grow from left to right
    speedometerFill_->position.x = SPEEDOMETER_X_POS - ((SPEEDOMETER_BAR_WIDTH / 2.0f) * (1.0f - speedRatio));

    // Update seven-segment display
    int speedInt = static_cast<int>(displaySpeed);
    std::array<int, 3> digits = {
        (speedInt / 100) % 10,  // hundreds
        (speedInt / 10) % 10,   // tens
        speedInt % 10           // ones
    };

    for (int d = 0; d < NUM_DIGITS; ++d) {
        int digit = digits[static_cast<size_t>(d)];

        // Hide leading zeros
        bool hideDigit = (d == 0 && speedInt < 100) || (d == 1 && speedInt < 10);

        if (hideDigit) {
            for (int s = 0; s < NUM_SEGMENTS_PER_DIGIT; ++s) {
                digitSegments_[static_cast<size_t>(d)][static_cast<size_t>(s)]->visible = false;
            }
        } else {
            // Show segments based on digit pattern
            for (int s = 0; s < NUM_SEGMENTS_PER_DIGIT; ++s) {
                digitSegments_[static_cast<size_t>(d)][static_cast<size_t>(s)]->visible = SEGMENT_PATTERNS[static_cast<size_t>(digit)][static_cast<size_t>(s)];
            }
        }
    }
}

void UIManager::render(const Vehicle& vehicle, const WindowSize& size) {
    float speed = vehicle.getVelocity();
    updateSpeedometer(speed);

    // Update nitrous indicator
    if (vehicle.hasNitrous()) {
        nitrousIndicator_->visible = true;
        nitrousBarOutline_->visible = false;
        nitrousBar_->visible = false;
    } else if (vehicle.isNitrousActive()) {
        nitrousIndicator_->visible = false;
        nitrousBarOutline_->visible = true;
        nitrousBar_->visible = true;

        // Update nitrous bar based on time remaining
        float timeRemaining = vehicle.getNitrousTimeRemaining();
        float nitrousRatio = timeRemaining / NITROUS_DURATION;

        nitrousBar_->scale.x = nitrousRatio;
        nitrousBar_->position.x = NITROUS_X_POS - ((NITROUS_BAR_WIDTH / 2.0f) * (1.0f - nitrousRatio));
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
