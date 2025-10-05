#include "uiManager.hpp"
#include <cmath>
#include <array>

using namespace threepp;

// Seven-segment display patterns for digits 0-9
// Segments: top, top-right, bottom-right, bottom, bottom-left, top-left, middle
static const std::array<std::array<bool, 7>, 10> SEGMENT_PATTERNS = {{
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

UIManager::UIManager(GLRenderer& renderer)
    : renderer_(renderer) {
    hudScene_ = std::make_shared<Scene>();

    // Orthographic camera for 2D overlay
    hudCamera_ = std::make_shared<OrthographicCamera>(-1, 1, 1, -1, 0.1f, 10.0f);  // -1 to 1 = normalized screen space
    hudCamera_->position.z = 1;

    createSpeedometerGeometry();
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

    for (int i = 0; i < 3; i++) {
        createSevenSegmentDigit(digitStartX + i * digitSpacing, digitY, i);
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
    for (auto& seg : segments) {
        seg->visible = false;
    }
}

void UIManager::updateSpeedometer(float speed) {
    // Calculate speed display
    float maxSpeed = 250.0f;  // 250 km/h - high enough for max vehicle speed display
    float displaySpeed = std::abs(speed) * 10.0f;  // *10 converts units/sec to km/h (tuned scale)
    float speedRatio = std::min(displaySpeed / maxSpeed, 1.0f);  // Clamp to 0-1 range

    // Update fill bar scale
    speedometerFill_->scale.x = speedRatio;

    // Position bar to grow from left to right
    float barWidth = 0.32f;  // Same as geometry width
    speedometerFill_->position.x = 0.6f - (barWidth / 2.0f) * (1.0f - speedRatio);  // Offset calculation for left-to-right growth

    // Update seven-segment display
    int speedInt = static_cast<int>(displaySpeed);
    int hundreds = (speedInt / 100) % 10;
    int tens = (speedInt / 10) % 10;
    int ones = speedInt % 10;

    std::array<int, 3> digits = {hundreds, tens, ones};

    for (int d = 0; d < 3; d++) {
        int digit = digits[d];

        // Hide leading zeros
        bool hideDigit = (d == 0 && speedInt < 100) || (d == 1 && speedInt < 10);

        if (hideDigit) {
            for (int s = 0; s < 7; s++) {
                digitSegments_[d][s]->visible = false;
            }
        } else {
            // Show segments based on digit pattern
            for (int s = 0; s < 7; s++) {
                digitSegments_[d][s]->visible = SEGMENT_PATTERNS[digit][s];
            }
        }
    }
}

void UIManager::render(const Vehicle& vehicle, const WindowSize& size) {
    float speed = vehicle.getVelocity();
    updateSpeedometer(speed);

    // Render HUD overlay on top of main scene
    renderer_.autoClear = false;
    renderer_.render(*hudScene_, *hudCamera_);
    renderer_.autoClear = true;
}
