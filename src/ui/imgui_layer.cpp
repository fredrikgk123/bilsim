#include "imgui_layer.hpp"
#include "../core/vehicle_tuning.hpp"
#include <imgui.h>
#include <cmath>
#include <algorithm>
#include <string_view>
#include <cfloat>

ImGuiLayer::ImGuiLayer() = default;

static inline ImU32 toU32(const ImVec4 &c) noexcept {
    return IM_COL32(static_cast<int>(c.x * 255.0f),
                    static_cast<int>(c.y * 255.0f),
                    static_cast<int>(c.z * 255.0f),
                    static_cast<int>(c.w * 255.0f));
}

// Convert degrees to radians (constexpr helper)
static inline constexpr float deg2rad(float deg) noexcept { return deg * 0.017453292519943295769f; }

void ImGuiLayer::render(const IVehicleState& vehicle, const threepp::WindowSize& size) {
    if (!ImGui::GetCurrentContext()) return;

    // HUD constants
    constexpr float MAX_DISPLAY_SPEED_KMH = 150.0f;
    constexpr float MAX_RPM = 7000.0f;

    const float speedKmh = std::abs(vehicle.getVelocity()) * 3.6f;
    const float speedRatio = std::clamp(speedKmh / MAX_DISPLAY_SPEED_KMH, 0.0f, 1.0f);
    const float rpm = vehicle.getRPM();
    const float rpmRatio = std::clamp(rpm / MAX_RPM, 0.0f, 1.0f);
    const int gear = vehicle.getCurrentGear();

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    ImFont* font = ImGui::GetFont();

    // Gauge drawing helper (arc gauge with ticks and needle)
    auto drawGauge = [&](ImVec2 center, float radius, float ratio, const char* label, const char* valueText, ImU32 color, float scaleMax) {
        const std::string_view label_sv(label);
        const bool isSpeed = (label_sv == "km/h" || label_sv == "mph");
        const bool isRPM = (label_sv == "RPM");

        // scale many of the drawing constants with radius so the gauge is fully scalable
        const float thickness = std::max(4.0f, radius * 0.04f);
        const float a1 = deg2rad(-120.0f);
        const float a2 = deg2rad(120.0f);

        // Background circle
        dl->AddCircleFilled(center, radius, toU32(ImVec4(0.f, 0.f, 0.f, 0.45f)));

        // Outer arc (empty)
        dl->PathClear();
        dl->PathArcTo(center, radius - std::max(6.0f, radius * 0.06f), a1, a2, 64);
        dl->PathStroke(toU32(ImVec4(0.25f, 0.25f, 0.25f, 0.9f)), false, thickness);

        // Filled arc based on ratio
        if (ratio > 1e-6f) {
            dl->PathClear();
            dl->PathArcTo(center, radius - std::max(6.0f, radius * 0.06f), a1, a1 + (a2 - a1) * ratio, 64);
            dl->PathStroke(color, false, thickness + std::max(1.0f, radius * 0.02f));
        }

        // Ticks and numeric labels at major ticks
        constexpr int ticks = 12;
        for (int i = 0; i <= ticks; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(ticks);
            const float a = a1 + (a2 - a1) * t;
            const bool major = (i % 3 == 0);
            const float p1r = radius - std::max(8.0f, radius * 0.07f);
            const float p2r = radius - (major ? std::max(radius * 0.14f, 28.0f) : std::max(radius * 0.09f, 18.0f));
            const ImVec2 p1(center.x + std::cos(a) * p1r, center.y + std::sin(a) * p1r);
            const ImVec2 p2(center.x + std::cos(a) * p2r, center.y + std::sin(a) * p2r);

            dl->AddLine(p1, p2, toU32(ImVec4(0.95f, 0.95f, 0.95f, 0.9f)), major ? std::max(2.0f, radius * 0.03f) : std::max(1.0f, radius * 0.015f));

            if (!isSpeed && !isRPM && major) {
                char lbl[16] = {0};
                const int val = static_cast<int>(std::round(scaleMax * t));
                std::snprintf(lbl, sizeof(lbl), "%d", val);
                const ImVec2 lpos(center.x + std::cos(a) * (radius - std::max(44.0f, radius * 0.18f)) - 8.0f,
                                  center.y + std::sin(a) * (radius - std::max(44.0f, radius * 0.18f)) - 6.0f);
                const float tickFont = std::max(9.0f, radius * 0.10f);
                dl->AddText(font, tickFont, lpos, toU32(ImVec4(0.85f, 0.85f, 0.85f, 0.95f)), lbl);
            }
        }

        // Needle (triangular/filled for a car-dash feel)
        const float needleA = a1 + (a2 - a1) * ratio;
        const ImVec2 tip(center.x + std::cos(needleA) * (radius - std::max(20.0f, radius * 0.12f)),
                         center.y + std::sin(needleA) * (radius - std::max(20.0f, radius * 0.12f)));
        const float bw = std::max(4.0f, radius * 0.06f);
        const ImVec2 dir(std::cos(needleA), std::sin(needleA));
        const ImVec2 perp(-dir.y, dir.x);
        const ImVec2 b1(center.x + perp.x * bw, center.y + perp.y * bw);
        const ImVec2 b2(center.x - perp.x * bw, center.y - perp.y * bw);
        dl->AddTriangleFilled(b1, b2, tip, color);
        dl->AddLine(center, tip, toU32(ImVec4(1.0f, 0.85f, 0.85f, 0.95f)), std::max(1.0f, radius * 0.008f));
        dl->AddCircleFilled(center, std::max(6.0f, radius * 0.08f), toU32(ImVec4(0.12f, 0.12f, 0.12f, 1.0f)));
        dl->AddCircle(center, std::max(6.0f, radius * 0.08f), toU32(ImVec4(0.95f, 0.95f, 0.95f, 0.06f)), 12, std::max(1.0f, radius * 0.02f));

        // Label and value text
        if (isSpeed) {
            // Digital speed box inside the gauge
            const float boxW = radius * 0.56f;
            const float boxH = radius * 0.38f;
            const float vShift = radius * 0.24f;
            const ImVec2 boxCenter(center.x - radius * 0.36f, center.y + radius * 0.02f + vShift);
            const ImVec2 boxTL(boxCenter.x - boxW * 0.5f, boxCenter.y - boxH * 0.5f);
            const ImVec2 boxBR(boxCenter.x + boxW * 0.5f, boxCenter.y + boxH * 0.5f);
            const float boxR = std::max(4.0f, radius * 0.06f);

            dl->AddRectFilled(boxTL, boxBR, toU32(ImVec4(0.08f, 0.08f, 0.08f, 0.95f)), boxR);
            dl->AddRect(boxTL, boxBR, toU32(ImVec4(1.0f, 1.0f, 1.0f, 0.06f)), boxR, 0, std::max(1.0f, radius * 0.02f));

            float speedFont = std::max(22.0f, radius * 0.38f);
            const char* unit = (label_sv == "mph") ? "mph" : "km/h";
            const float unitFont = std::max(10.0f, radius * 0.12f);
            const ImVec2 unitSize = font->CalcTextSizeA(unitFont, FLT_MAX, 0.0f, unit);
            const float padding = std::max(4.0f, radius * 0.04f);
            const float availableWidth = boxW - padding * 2.0f - unitSize.x - padding * 0.5f;

            ImVec2 speedTxt = font->CalcTextSizeA(speedFont, FLT_MAX, 0.0f, valueText);
            if (speedTxt.x > availableWidth) {
                const float scale = availableWidth / speedTxt.x;
                speedFont = std::max(12.0f, speedFont * scale);
                speedTxt = font->CalcTextSizeA(speedFont, FLT_MAX, 0.0f, valueText);
            }

            const float speedX = boxCenter.x - boxW * 0.5f + padding;
            const float speedY = boxCenter.y - speedTxt.y * 0.6f;
            dl->AddText(font, speedFont, ImVec2(speedX, speedY), toU32(ImVec4(1, 1, 1, 0.98f)), valueText);

            const float unitX = boxCenter.x + boxW * 0.5f - padding - unitSize.x;
            const float unitY = boxCenter.y + boxH * 0.5f - padding - unitSize.y;
            dl->AddText(font, unitFont, ImVec2(unitX, unitY), toU32(ImVec4(0.7f, 0.7f, 0.7f, 0.95f)), unit);

        } else {
            if (!isRPM) {
                const float valueFont = std::max(14.0f, radius * 0.28f);
                const ImVec2 txtSize = font->CalcTextSizeA(valueFont, FLT_MAX, 0.0f, valueText);
                dl->AddText(font, valueFont, ImVec2(center.x - txtSize.x * 0.5f, center.y + radius * 0.18f - txtSize.y * 0.5f), toU32(ImVec4(1, 1, 1, 0.98f)), valueText);
            }
            if (!isRPM) {
                const float labelFont = std::max(10.0f, radius * 0.12f);
                const ImVec2 lblSize = font->CalcTextSizeA(labelFont, FLT_MAX, 0.0f, label);
                dl->AddText(font, labelFont, ImVec2(center.x - lblSize.x * 0.5f, center.y + radius * 0.33f - lblSize.y * 0.5f), toU32(ImVec4(0.8f, 0.8f, 0.8f, 0.9f)), label);
            }
        }
    };

    // Pixel positions
    const int w = size.width();
    const int h = size.height();

    // Smooth needles (interpolate towards target ratios)
    displayedSpeedRatio_ += (speedRatio - displayedSpeedRatio_) * smoothingAlpha_;
    displayedRpmRatio_ += (rpmRatio - displayedRpmRatio_) * smoothingAlpha_;

    // Adaptive gauge sizing based on the window's minimum dimension
    const float minDim = std::min(static_cast<float>(w), static_cast<float>(h));
    const float gaugeRadius = std::clamp(minDim * 0.12f, minDim * 0.06f, minDim * 0.18f);
    const float gap = gaugeRadius * 0.32f;

    // Instrument cluster background (rounded rect placed at lower-right)
    const float clusterHalfW = (gaugeRadius * 1.9f + gap);
    const float rightMargin = std::max(6.0f, minDim * 0.02f);
    const float bottomMargin = std::max(6.0f, minDim * 0.03f);
    const ImVec2 clusterCenter(static_cast<float>(w) - clusterHalfW - rightMargin,
                               static_cast<float>(h) - (gaugeRadius + bottomMargin));
    const ImVec2 clusterTL(clusterCenter.x - clusterHalfW, clusterCenter.y - (gaugeRadius * 0.9f));
    const ImVec2 clusterBR(clusterCenter.x + clusterHalfW, clusterCenter.y + (gaugeRadius * 0.45f));
    const float clusterRounding = std::max(6.0f, gaugeRadius * 0.1f);
    const float clusterBorder = std::max(1.0f, gaugeRadius * 0.02f);

    dl->AddRectFilled(clusterTL, clusterBR, toU32(ImVec4(0.02f, 0.02f, 0.02f, 0.55f)), clusterRounding);
    dl->AddRect(clusterTL, clusterBR, toU32(ImVec4(1.0f, 1.0f, 1.0f, 0.03f)), clusterRounding, 0, clusterBorder);

    // Positions for left (RPM) and right (speed) gauges within cluster
    const ImVec2 leftCenter(clusterCenter.x - (gaugeRadius + gap * 0.45f), clusterCenter.y);
    const ImVec2 rightCenter(clusterCenter.x + (gaugeRadius + gap * 0.45f), clusterCenter.y);

    // Draw RPM (left) and Speed (right) using smoothed ratio
    {
        char speedbuf[32] = {0};
        std::snprintf(speedbuf, sizeof(speedbuf), "%d", static_cast<int>(std::round(speedKmh)));
        drawGauge(rightCenter, gaugeRadius, displayedSpeedRatio_, "km/h", speedbuf, toU32(ImVec4(0.2f, 0.9f, 0.2f, 1.0f)), MAX_DISPLAY_SPEED_KMH);

        char rpmbuf[32] = {0};
        std::snprintf(rpmbuf, sizeof(rpmbuf), "%d", static_cast<int>(std::round(rpm)));
        drawGauge(leftCenter, gaugeRadius, displayedRpmRatio_, "RPM", rpmbuf, toU32(ImVec4(1.0f, 0.6f, 0.0f, 1.0f)), MAX_RPM);
    }

    // Gear display: moved inside the RPM (left) gauge for a typical car HUD
    {
        const float gearRadius = gaugeRadius * 0.42f;
        const ImVec2 center(leftCenter.x, leftCenter.y + gaugeRadius * 0.06f);
        dl->AddCircleFilled(center, gearRadius, toU32(ImVec4(0.04f, 0.04f, 0.04f, 0.95f)));
        dl->AddCircle(center, gearRadius, toU32(ImVec4(0.95f, 0.95f, 0.95f, 0.06f)), 32, std::max(1.5f, gaugeRadius * 0.04f));
        const std::string gearStr = (gear == 0) ? "R" : (gear > 0 ? std::to_string(gear) : std::string("-"));
        const float gearFont = std::max(18.0f, gaugeRadius * 0.5f);
        const ImVec2 txtSize = font->CalcTextSizeA(gearFont, FLT_MAX, 0.0f, gearStr.c_str());
        dl->AddText(font, gearFont, ImVec2(center.x - txtSize.x * 0.5f, center.y - txtSize.y * 0.5f), toU32(ImVec4(0.98f, 0.98f, 0.98f, 1.0f)), gearStr.c_str());
    }

    // Nitrous indicator: small circular radial above the gear
    {
        const float nitroRadius = gaugeRadius * 0.22f;
        const ImVec2 speedBoxCenter(rightCenter.x - gaugeRadius * 0.36f, rightCenter.y + gaugeRadius * 0.02f + gaugeRadius * 0.24f);
        const float boxH = gaugeRadius * 0.38f;
        const ImVec2 center(speedBoxCenter.x, speedBoxCenter.y - (boxH * 0.5f) - nitroRadius - 6.0f);

        dl->AddCircleFilled(center, nitroRadius, toU32(ImVec4(0.06f, 0.06f, 0.08f, 0.75f)));
        dl->AddCircle(center, nitroRadius, toU32(ImVec4(0.9f, 0.9f, 0.9f, 0.06f)), 32, 1.5f);

        if (vehicle.isNitrousActive()) {
            const float remaining = vehicle.getNitrousTimeRemaining();
            const float ratio = std::clamp(remaining / VehicleTuning::NITROUS_DURATION, 0.0f, 1.0f);
            const float a1n = deg2rad(-90.0f);
            const float a2n = deg2rad(-90.0f + 360.0f * ratio);
            dl->PathClear();
            dl->PathArcTo(center, nitroRadius - 6.0f, a1n, a2n, 64);
            dl->PathStroke(toU32(ImVec4(0.2f, 0.6f, 1.0f, 1.0f)), false, 5.0f);
            dl->AddText(font, 12.0f, ImVec2(center.x - 18.0f, center.y - 6.0f), toU32(ImVec4(1, 1, 1, 1)), "BOOST");

        } else if (vehicle.hasNitrous()) {
            dl->AddText(font, 14.0f, ImVec2(center.x - 22.0f, center.y - 6.0f), toU32(ImVec4(0.4f, 0.8f, 1.0f, 1.0f)), "NITRO");

        } else {
            dl->AddText(font, 12.0f, ImVec2(center.x - 15.0f, center.y - 6.0f), toU32(ImVec4(0.6f, 0.6f, 0.6f, 1.0f)), "---");
        }
    }


}

