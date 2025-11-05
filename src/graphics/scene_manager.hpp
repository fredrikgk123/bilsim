#pragma once

#include <threepp/threepp.hpp>
#include <memory>

// Camera modes
enum class CameraMode {
    FOLLOW,   // Third-person follow camera
    HOOD,     // First-person hood camera
    SIDE,     // Side view camera
    INSIDE    // Interior / cockpit camera
};

class SceneManager {
public:
    SceneManager();
    ~SceneManager() = default;

    // Non-copyable (manages renderer and scene resources)
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    // Scene component access
    [[nodiscard]] threepp::Scene& getScene() noexcept;
    [[nodiscard]] threepp::Camera& getCamera() noexcept;
    [[nodiscard]] threepp::GLRenderer& getRenderer() noexcept;

    // Setup methods
    void setupLighting();
    void setupGround();
    void setupCamera(float aspectRatio);
    void setupRenderer(const threepp::WindowSize& size);
    void setupMinimapCamera(float aspectRatio);

    // Camera control
    void updateCameraFollowTarget(float targetX, float targetY, float targetZ, float targetRotation,
                                  float vehicleScale, bool nitrousActive = false, float vehicleVelocity = 0.0f,
                                  float driftAngle = 0.0f);
    void updateMinimapCamera(float targetX, float targetZ, float vehicleScale);
    void updateCameraFOV(bool nitrousActive, float vehicleVelocity = 0.0f);

    // Camera mode switching
    void setCameraMode(CameraMode mode) noexcept;
    [[nodiscard]] CameraMode getCameraMode() const noexcept;
    void toggleCameraMode() noexcept;

    // Rendering
    void render();
    void renderMinimap();
    void resize(const threepp::WindowSize& size);

private:
    std::unique_ptr<threepp::GLRenderer> renderer_;
    std::shared_ptr<threepp::Scene> scene_;
    std::shared_ptr<threepp::PerspectiveCamera> camera_;
    std::shared_ptr<threepp::OrthographicCamera> minimapCamera_;
    std::shared_ptr<threepp::Mesh> groundMesh_;

    // Camera follow parameters
    float cameraDistance_;
    float cameraHeight_;
    float cameraLerpSpeed_;

    // Side view camera parameters
    float cameraSideDistance_; // Distance from vehicle for side view
    float cameraSideHeight_;   // Height for side view

    // Inside/cockpit camera parameters
    float cameraInsideForwardOffset_; // Forward offset for interior camera
    float cameraInsideHeight_;        // Height for interior camera
    float cameraInsideSideOffset_;    // Lateral offset (right/left) for interior camera (behind wheel)

    // FOV parameters
    float baseFOV_;
    float currentFOV_;
    float targetFOV_;
    float fovLerpSpeed_;

    // Camera mode
    CameraMode cameraMode_;

    // Current camera state for smooth interpolation
    float currentCameraX_;
    float currentCameraY_;
    float currentCameraZ_;
    float currentLookAtX_;
    float currentLookAtY_;
    float currentLookAtZ_;

    // Drift camera state

    // Vehicle scale tracking for camera adjustments
    float currentVehicleScale_;
    float driftCameraOffset_;  // Current side offset for drift camera
};
