#include "scene_manager.hpp"
#include <cmath>

using namespace threepp;

// Anonymous namespace for file-local constants
namespace {
    // Base camera distances (multiplied by vehicle scale)
    constexpr float BASE_CAMERA_DISTANCE = 8.0f;
    constexpr float BASE_CAMERA_HEIGHT = 4.0f;
    constexpr float BASE_CAMERA_LERP_SPEED = 0.2f;

    // Minimap configuration
    constexpr float BASE_MINIMAP_VIEW_SIZE = 15.0f;
    constexpr float MINIMAP_HEIGHT = 50.0f;
    constexpr float MINIMAP_SCALE_MULTIPLIER = 1.5f;  // How much vehicle scale affects minimap view

    // Hood cam offsets (relative to vehicle scale)
    constexpr float HOOD_CAM_FORWARD_OFFSET_BASE = 2.5f;
    constexpr float HOOD_CAM_HEIGHT_BASE = 1.3f;
    constexpr float HOOD_CAM_LOOK_DISTANCE_BASE = 10.0f;

    // Side cam parameters (relative to vehicle scale)
    constexpr float SIDE_CAM_DISTANCE_BASE = 6.0f;
    constexpr float SIDE_CAM_HEIGHT_BASE = 2.0f;

    // Inside/cockpit cam offsets (relative to vehicle scale)
    constexpr float INSIDE_CAM_FORWARD_OFFSET_BASE = -0.2f;   // Inside the car (positive = forward)
    constexpr float INSIDE_CAM_HEIGHT_BASE = 1.05f;          // Eye level height
    constexpr float INSIDE_CAM_SIDE_OFFSET_BASE = -0.3f;     // Left side (negative = left for driver)
    constexpr float INSIDE_CAM_LOOK_DISTANCE_BASE = 10.0f;

    // Drift camera parameters (relative to vehicle scale)
    constexpr float DRIFT_SIDE_OFFSET_MAX_BASE = 1.5f;
    constexpr float DRIFT_ANGLE_THRESHOLD = 0.2f;
    constexpr float DRIFT_OFFSET_LERP_SPEED = 0.12f;
    constexpr float DRIFT_ANGLE_NORMALIZATION = 1.0f;

    // Camera interpolation parameters
    constexpr float VELOCITY_LERP_MAX_SPEED = 30.0f;
    constexpr float VELOCITY_LERP_SPEED_FACTOR = 0.12f;
    constexpr float LOOK_AT_LERP_SPEED_MULTIPLIER = 1.15f;

    // Ground/Grid constants
    constexpr float GROUND_SIZE = 200.0f;
    constexpr int GRID_DIVISIONS = static_cast<int>(GROUND_SIZE);
    constexpr float GRID_Z_OFFSET = 0.01f;

    // Camera FOV constants
    constexpr float CAMERA_FOV_MIN = 75.0f;
    constexpr float CAMERA_FOV_MAX = 95.0f;
    constexpr float NITROUS_FOV_BOOST = 10.0f;
    constexpr float FOV_LERP_SPEED = 0.08f;
    constexpr float SPEED_FOR_MAX_FOV = 30.0f;
    constexpr float CAMERA_NEAR = 0.1f;
    constexpr float CAMERA_FAR = 1000.0f;

    // Cockpit/inside camera FOV tuning
    constexpr float COCKPIT_FOV_FACTOR = 0.5f;
    constexpr float COCKPIT_NITROUS_FACTOR = 0.5f;
    constexpr float COCKPIT_FOV_LERP_MULT = 0.6f;
    constexpr float NITROUS_FOV_LERP_MULT = 0.25f;

    // Lighting constants
    constexpr unsigned int AMBIENT_COLOR = 0x404040;
    constexpr float AMBIENT_INTENSITY = 1.0f;
    constexpr unsigned int DIRECTIONAL_COLOR = 0xffffff;
    constexpr float DIRECTIONAL_INTENSITY = 0.8f;
    constexpr float SHADOW_AREA_SIZE = 100.0f;
    constexpr float DIRECTIONAL_LIGHT_HEIGHT = 50.0f;
    constexpr int SHADOW_MAP_SIZE = 4096;
}

SceneManager::SceneManager()
    : cameraDistance_(BASE_CAMERA_DISTANCE),
      cameraHeight_(BASE_CAMERA_HEIGHT),
      cameraLerpSpeed_(BASE_CAMERA_LERP_SPEED),
      cameraSideDistance_(SIDE_CAM_DISTANCE_BASE),
      cameraSideHeight_(SIDE_CAM_HEIGHT_BASE),
      cameraInsideForwardOffset_(INSIDE_CAM_FORWARD_OFFSET_BASE),
      cameraInsideHeight_(INSIDE_CAM_HEIGHT_BASE),
      cameraInsideSideOffset_(INSIDE_CAM_SIDE_OFFSET_BASE),
      baseFOV_(CAMERA_FOV_MIN),
      currentFOV_(CAMERA_FOV_MIN),
      targetFOV_(CAMERA_FOV_MIN),
      fovLerpSpeed_(FOV_LERP_SPEED),
      cameraMode_(CameraMode::FOLLOW),
      currentCameraX_(0.0f),
      currentCameraY_(BASE_CAMERA_HEIGHT),
      currentCameraZ_(0.0f),
      currentLookAtX_(0.0f),
      currentLookAtY_(0.0f),
      currentLookAtZ_(0.0f),
      driftCameraOffset_(0.0f),
      currentVehicleScale_(1.0f),
      cameraYawOffset_(0.0f),
      targetCameraYawOffset_(0.0f),
      yawLerpSpeed_(0.1f) {
    scene_ = std::make_shared<Scene>();
    renderer_ = std::make_unique<GLRenderer>();
    renderer_->shadowMap().enabled = true;
}

Scene& SceneManager::getScene() noexcept {
    return *scene_;
}

Camera& SceneManager::getCamera() noexcept {
    return *camera_;
}

GLRenderer& SceneManager::getRenderer() noexcept {
    return *renderer_;
}

void SceneManager::setupLighting() {
    // Ambient light for base illumination
    auto ambientLight = AmbientLight::create(AMBIENT_COLOR, AMBIENT_INTENSITY);
    scene_->add(ambientLight);
    
    // Directional light for main lighting and shadows
    auto directionalLight = DirectionalLight::create(DIRECTIONAL_COLOR, DIRECTIONAL_INTENSITY);
    directionalLight->position.set(0, DIRECTIONAL_LIGHT_HEIGHT, 0);
    directionalLight->castShadow = true;

    // Configure shadow camera
    auto shadowCamera = directionalLight->shadow->camera->as<OrthographicCamera>();
    shadowCamera->left = -SHADOW_AREA_SIZE;
    shadowCamera->right = SHADOW_AREA_SIZE;
    shadowCamera->top = SHADOW_AREA_SIZE;
    shadowCamera->bottom = -SHADOW_AREA_SIZE;
    shadowCamera->updateProjectionMatrix();

    // High resolution shadow map
    directionalLight->shadow->mapSize.set(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

    scene_->add(directionalLight);
}

void SceneManager::setupGround() {
    // Create ground plane
    auto groundGeometry = PlaneGeometry::create(GROUND_SIZE, GROUND_SIZE);
    auto groundMaterial = MeshPhongMaterial::create();
    groundMaterial->color = Color(0x3a7d44);

    groundMesh_ = Mesh::create(groundGeometry, groundMaterial);
    groundMesh_->rotation.x = -math::PI / 2;
    groundMesh_->receiveShadow = true;
    scene_->add(groundMesh_);

    // Add grid helper for visual reference
    auto grid = GridHelper::create(GROUND_SIZE, GRID_DIVISIONS, 0x2d5a33, 0x2d5a33);
    grid->position.y = GRID_Z_OFFSET;
    scene_->add(grid);
}

void SceneManager::setupCamera(float aspectRatio) {
    camera_ = std::make_shared<PerspectiveCamera>(CAMERA_FOV_MIN, aspectRatio, CAMERA_NEAR, CAMERA_FAR);
    camera_->position.set(currentCameraX_, currentCameraY_, currentCameraZ_);
}

void SceneManager::setupRenderer(const WindowSize& size) {
    renderer_->setSize(size);
    renderer_->setClearColor(Color::aliceblue);
}

void SceneManager::updateCameraFollowTarget(float targetX, float targetY, float targetZ, float targetRotation,
                                           float vehicleScale, bool nitrousActive, float vehicleVelocity, float driftAngle) {
    currentVehicleScale_ = vehicleScale;

    // Lerp the camera yaw offset for smooth transitions
    cameraYawOffset_ += (targetCameraYawOffset_ - cameraYawOffset_) * yawLerpSpeed_;

    const float scaledCameraDistance = BASE_CAMERA_DISTANCE * vehicleScale;
    const float scaledCameraHeight = BASE_CAMERA_HEIGHT * vehicleScale;
    const float scaledHoodCamForwardOffset = HOOD_CAM_FORWARD_OFFSET_BASE * vehicleScale;
    const float scaledHoodCamHeight = HOOD_CAM_HEIGHT_BASE * vehicleScale;
    const float scaledHoodCamLookDistance = HOOD_CAM_LOOK_DISTANCE_BASE * vehicleScale;
    const float scaledSideCamDistance = SIDE_CAM_DISTANCE_BASE * vehicleScale;
    const float scaledSideCamHeight = SIDE_CAM_HEIGHT_BASE * vehicleScale;
    const float scaledDriftSideOffsetMax = DRIFT_SIDE_OFFSET_MAX_BASE * vehicleScale;

    // Inside camera offsets (scaled by vehicle size)
    const float scaledInsideCamForwardOffset = INSIDE_CAM_FORWARD_OFFSET_BASE * vehicleScale;
    const float scaledInsideCamHeight = INSIDE_CAM_HEIGHT_BASE * vehicleScale;
    const float scaledInsideCamSideOffset = INSIDE_CAM_SIDE_OFFSET_BASE * vehicleScale;
    const float scaledInsideCamLookDistance = INSIDE_CAM_LOOK_DISTANCE_BASE * vehicleScale;

    float desiredCameraX, desiredCameraY, desiredCameraZ;
    float desiredLookAtX, desiredLookAtY, desiredLookAtZ;

    if (cameraMode_ == CameraMode::INTERIOR) {
        // Inside/cockpit cam - driver's eye position
        desiredCameraX = targetX + (std::sin(targetRotation) * scaledInsideCamForwardOffset);
        desiredCameraY = targetY + scaledInsideCamHeight;
        desiredCameraZ = targetZ + (std::cos(targetRotation) * scaledInsideCamForwardOffset);

        // Apply lateral offset for right-side driving position
        const float sideAngle = targetRotation + (math::PI / 2.0f);
        desiredCameraX += std::sin(sideAngle) * scaledInsideCamSideOffset;
        desiredCameraZ += std::cos(sideAngle) * scaledInsideCamSideOffset;

        // Look ahead from cabin, adjusted by yaw offset
        desiredLookAtX = targetX + (std::sin(targetRotation + cameraYawOffset_) * scaledInsideCamLookDistance);
        desiredLookAtY = targetY + scaledInsideCamHeight;
        desiredLookAtZ = targetZ + (std::cos(targetRotation + cameraYawOffset_) * scaledInsideCamLookDistance);

        driftCameraOffset_ = 0.0f;

        // Immediate positioning for cockpit camera (no interpolation)
        camera_->position.set(desiredCameraX, desiredCameraY, desiredCameraZ);
        camera_->lookAt(desiredLookAtX, desiredLookAtY, desiredLookAtZ);

        // Update internal state
        currentCameraX_ = desiredCameraX;
        currentCameraY_ = desiredCameraY;
        currentCameraZ_ = desiredCameraZ;
        currentLookAtX_ = desiredLookAtX;
        currentLookAtY_ = desiredLookAtY;
        currentLookAtZ_ = desiredLookAtZ;

        return;
    } else {
        // Follow cam with drift offset and yaw adjustment
        const float absDriftAngle = std::abs(driftAngle);
        float targetDriftOffset = 0.0f;

        if (absDriftAngle > DRIFT_ANGLE_THRESHOLD) {
            const float driftIntensity = std::min(absDriftAngle / DRIFT_ANGLE_NORMALIZATION, 1.0f);
            targetDriftOffset = driftIntensity * scaledDriftSideOffsetMax;
            targetDriftOffset *= (driftAngle > 0) ? 1.0f : -1.0f;
        }

        // Smoothly interpolate drift offset
        driftCameraOffset_ += (targetDriftOffset - driftCameraOffset_) * DRIFT_OFFSET_LERP_SPEED;

        // Calculate perpendicular direction for side offset
        const float sideAngle = targetRotation + (math::PI / 2.0f);

        // Follow cam - position behind and above vehicle, adjusted by yaw offset
        const float adjustedRotation = targetRotation + cameraYawOffset_;
        desiredCameraX = targetX - (std::sin(adjustedRotation) * scaledCameraDistance) +
                        (std::sin(sideAngle) * driftCameraOffset_);
        desiredCameraY = targetY + scaledCameraHeight;
        desiredCameraZ = targetZ - (std::cos(adjustedRotation) * scaledCameraDistance) +
                        (std::cos(sideAngle) * driftCameraOffset_);

        // Look at the vehicle
        desiredLookAtX = targetX;
        desiredLookAtY = targetY;
        desiredLookAtZ = targetZ;
    }

    // Velocity-adaptive camera interpolation
    const float speedFactor = std::min(std::abs(vehicleVelocity) / VELOCITY_LERP_MAX_SPEED, 1.0f);
    const float adaptiveLerpSpeed = cameraLerpSpeed_ + (speedFactor * VELOCITY_LERP_SPEED_FACTOR);

    // Smoothly interpolate camera position
    currentCameraX_ += (desiredCameraX - currentCameraX_) * adaptiveLerpSpeed;
    currentCameraY_ += (desiredCameraY - currentCameraY_) * adaptiveLerpSpeed;
    currentCameraZ_ += (desiredCameraZ - currentCameraZ_) * adaptiveLerpSpeed;

    // Smoothly interpolate look-at target
    const float lookAtLerpSpeed = adaptiveLerpSpeed * LOOK_AT_LERP_SPEED_MULTIPLIER;
    currentLookAtX_ += (desiredLookAtX - currentLookAtX_) * lookAtLerpSpeed;
    currentLookAtY_ += (desiredLookAtY - currentLookAtY_) * lookAtLerpSpeed;
    currentLookAtZ_ += (desiredLookAtZ - currentLookAtZ_) * lookAtLerpSpeed;

    // Apply smoothed position and look-at
    camera_->position.set(currentCameraX_, currentCameraY_, currentCameraZ_);
    camera_->lookAt(currentLookAtX_, currentLookAtY_, currentLookAtZ_);
}

void SceneManager::updateMinimapCamera(float targetX, float targetZ, float vehicleScale) {
    // Keep minimap centered on vehicle
    minimapCamera_->position.set(targetX, MINIMAP_HEIGHT, targetZ);
    minimapCamera_->lookAt(targetX, 0.0f, targetZ);

    // Scale view based on vehicle scale only
    const float scaledViewSize = BASE_MINIMAP_VIEW_SIZE * (1.0f + (vehicleScale - 1.0f) * MINIMAP_SCALE_MULTIPLIER);

    minimapCamera_->left = -scaledViewSize;
    minimapCamera_->right = scaledViewSize;
    minimapCamera_->top = scaledViewSize;
    minimapCamera_->bottom = -scaledViewSize;
    minimapCamera_->updateProjectionMatrix();
}

void SceneManager::updateCameraFOV(bool nitrousActive, float vehicleVelocity) {
    // Calculate base speed ratio (0..1)
    float speedRatio = std::min(vehicleVelocity, SPEED_FOR_MAX_FOV) / SPEED_FOR_MAX_FOV;

    // Base FOV calculation (speed-based portion)
    float fovRange = CAMERA_FOV_MAX - CAMERA_FOV_MIN;

    float targetFOV = CAMERA_FOV_MIN + (speedRatio * fovRange);

    // Apply nitrous boost normally
    if (nitrousActive) {
        targetFOV += NITROUS_FOV_BOOST;
    }

    // Adjust the target FOV for cockpit mode (reduced magnitude)
    if (cameraMode_ == CameraMode::INTERIOR) {
        targetFOV = CAMERA_FOV_MIN + (speedRatio * fovRange * COCKPIT_FOV_FACTOR);
        if (nitrousActive) targetFOV += (NITROUS_FOV_BOOST * COCKPIT_NITROUS_FACTOR);
    }

    // Compute lerp speed and reduce it when nitrous is active to make the transition smoother
    float lerpSpeed = fovLerpSpeed_;
    if (cameraMode_ == CameraMode::INTERIOR) lerpSpeed *= COCKPIT_FOV_LERP_MULT;
    if (nitrousActive) lerpSpeed *= NITROUS_FOV_LERP_MULT;

    // Interpolate current FOV toward target using the computed lerpSpeed (applies also to cockpit now)
    currentFOV_ += (targetFOV - currentFOV_) * lerpSpeed;

    // Apply FOV to camera
    camera_->fov = currentFOV_;
    camera_->updateProjectionMatrix();
}

void SceneManager::render() {
    renderer_->render(*scene_, *camera_);
}

void SceneManager::resize(const WindowSize& size) {
    camera_->aspect = size.aspect();
    camera_->updateProjectionMatrix();
    renderer_->setSize(size);
}

void SceneManager::setupMinimapCamera(float aspectRatio) {
    // Orthographic camera for top-down minimap view
    minimapCamera_ = std::make_shared<OrthographicCamera>(
        -BASE_MINIMAP_VIEW_SIZE * aspectRatio, BASE_MINIMAP_VIEW_SIZE * aspectRatio,
        BASE_MINIMAP_VIEW_SIZE, -BASE_MINIMAP_VIEW_SIZE,
        CAMERA_NEAR, CAMERA_FAR
    );

    // Position camera above scene looking down
    minimapCamera_->position.set(0, MINIMAP_HEIGHT, 0);
}

void SceneManager::setCameraMode(CameraMode mode) noexcept {
    cameraMode_ = mode;
}

CameraMode SceneManager::getCameraMode() const noexcept {
    return cameraMode_;
}

void SceneManager::toggleCameraMode() noexcept {
    if (cameraMode_ == CameraMode::FOLLOW) {
        cameraMode_ = CameraMode::INTERIOR;
    } else {
        cameraMode_ = CameraMode::FOLLOW;
    }
}

void SceneManager::renderMinimap() {
    renderer_->render(*scene_, *minimapCamera_);
}

void SceneManager::adjustCameraYaw(float delta) {
    targetCameraYawOffset_ += delta;
}

void SceneManager::setCameraYaw(float yaw) {
    targetCameraYawOffset_ = yaw;
    cameraYawOffset_ = yaw;
}

void SceneManager::setCameraYawTarget(float yaw) {
    targetCameraYawOffset_ = yaw;
}
