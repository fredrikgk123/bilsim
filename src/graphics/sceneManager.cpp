#include "sceneManager.hpp"
#include <cmath>

using namespace threepp;

// Anonymous namespace - these constants are LOCAL to this file only (NOT global!)
namespace {
    // Camera constants
    constexpr float DEFAULT_CAMERA_DISTANCE = 8.0f;   // 8 units behind - far enough to see surroundings
    constexpr float DEFAULT_CAMERA_HEIGHT = 4.0f;     // 4 units above - provides good viewing angle
    constexpr float DEFAULT_CAMERA_LERP_SPEED = 0.1f; // 0.1 = smooth, 1.0 = instant - creates cinematic camera movement
    constexpr float MINIMAP_VIEW_SIZE = 15.0f;        // 15 units - covers 30x30 area
    constexpr float MINIMAP_HEIGHT = 50.0f;           // 50 units high - bird's eye view

    // Hood cam constants
    constexpr float HOOD_CAM_FORWARD_OFFSET = 2.5f;  // 2.5 units in front of vehicle center (moved more forward)
    constexpr float HOOD_CAM_HEIGHT = 1.3f;          // 1.3 units above ground (0.5 car lift + 0.8 hood height)
    constexpr float HOOD_CAM_LOOK_DISTANCE = 10.0f;  // Look 10 units ahead

    // Ground/Grid constants
    constexpr float GROUND_SIZE = 200.0f;             // 200x200 units - large play area
    constexpr int GRID_DIVISIONS = static_cast<int>(GROUND_SIZE); // Same as ground size - 1 unit per grid square
    constexpr float GRID_Z_OFFSET = 0.01f;            // 0.01 units above ground - prevents z-fighting flickering

    // Camera FOV constants
    constexpr float CAMERA_FOV = 75.0f;               // FOV=75° (wide for awareness)
    constexpr float NITROUS_FOV = 82.0f;              // FOV=82° during nitrous boost (wider = faster feel)
    constexpr float FOV_LERP_SPEED = 0.05f;           // Smooth FOV transitions
    constexpr float CAMERA_NEAR = 0.1f;
    constexpr float CAMERA_FAR = 1000.0f;

    // Lighting constants
    constexpr unsigned int AMBIENT_COLOR = 0x404040;  // Dim gray - prevents completely black shadows
    constexpr float AMBIENT_INTENSITY = 1.0f;
    constexpr unsigned int DIRECTIONAL_COLOR = 0xffffff;
    constexpr float DIRECTIONAL_INTENSITY = 0.8f;     // 80% intensity - bright but not blown out
    constexpr float SHADOW_AREA_SIZE = 200.0f;        // Shadow area: 400x400 units (covers entire play area)
}

SceneManager::SceneManager()
    : cameraDistance_(DEFAULT_CAMERA_DISTANCE),
      cameraHeight_(DEFAULT_CAMERA_HEIGHT),
      cameraLerpSpeed_(DEFAULT_CAMERA_LERP_SPEED),
      baseFOV_(CAMERA_FOV),
      currentFOV_(CAMERA_FOV),
      targetFOV_(CAMERA_FOV),
      fovLerpSpeed_(FOV_LERP_SPEED),
      cameraMode_(CameraMode::FOLLOW),
      currentCameraX_(0.0f),
      currentCameraY_(DEFAULT_CAMERA_HEIGHT),
      currentCameraZ_(DEFAULT_CAMERA_DISTANCE),
      currentLookAtX_(0.0f),
      currentLookAtY_(0.0f),
      currentLookAtZ_(0.0f) {
    scene_ = std::make_shared<Scene>();
    renderer_ = std::make_unique<GLRenderer>();
    renderer_->shadowMap().enabled = true;
}

Scene& SceneManager::getScene() {
    return *scene_;
}

Camera& SceneManager::getCamera() {
    return *camera_;
}

GLRenderer& SceneManager::getRenderer() {
    return *renderer_;
}

void SceneManager::setupLighting() {
    // Ambient light for base illumination
    auto ambientLight = AmbientLight::create(AMBIENT_COLOR, AMBIENT_INTENSITY);
    scene_->add(ambientLight);
    
    // Directional light for main lighting and shadows
    auto directionalLight = DirectionalLight::create(DIRECTIONAL_COLOR, DIRECTIONAL_INTENSITY);
    directionalLight->position.set(5, 10, 7);
    directionalLight->castShadow = true;

    // Configure shadow camera
    auto shadowCamera = directionalLight->shadow->camera->as<OrthographicCamera>();
    shadowCamera->left = -SHADOW_AREA_SIZE;
    shadowCamera->right = SHADOW_AREA_SIZE;
    shadowCamera->top = SHADOW_AREA_SIZE;
    shadowCamera->bottom = -SHADOW_AREA_SIZE;
    shadowCamera->updateProjectionMatrix();

    scene_->add(directionalLight);
}

void SceneManager::setupGround() {
    // Create ground plane
    auto groundGeometry = PlaneGeometry::create(GROUND_SIZE, GROUND_SIZE);
    auto groundMaterial = MeshPhongMaterial::create();
    groundMaterial->color = Color::gray;
    
    groundMesh_ = Mesh::create(groundGeometry, groundMaterial);
    groundMesh_->rotation.x = -math::PI / 2;
    groundMesh_->receiveShadow = true;
    scene_->add(groundMesh_);

    // Add grid helper for visual reference
    auto grid = GridHelper::create(GROUND_SIZE, GRID_DIVISIONS);
    grid->position.y = GRID_Z_OFFSET;
    scene_->add(grid);
}

void SceneManager::setupCamera(float aspectRatio) {
    camera_ = std::make_shared<PerspectiveCamera>(CAMERA_FOV, aspectRatio, CAMERA_NEAR, CAMERA_FAR);
    camera_->position.set(currentCameraX_, currentCameraY_, currentCameraZ_);
    camera_->lookAt(Vector3(currentLookAtX_, currentLookAtY_, currentLookAtZ_));
}

void SceneManager::setupRenderer(const WindowSize& size) {
    renderer_->setSize(size);
    renderer_->setClearColor(Color::aliceblue);
}

void SceneManager::updateCameraFollowTarget(float targetX, float targetY, float targetZ, float targetRotation, bool nitrousActive) {
    float desiredCameraX, desiredCameraY, desiredCameraZ;
    float desiredLookAtX, desiredLookAtY, desiredLookAtZ;

    if (cameraMode_ == CameraMode::HOOD) {
        // Hood cam - position camera at hood level, slightly forward
        desiredCameraX = targetX + (std::sin(targetRotation) * HOOD_CAM_FORWARD_OFFSET);
        desiredCameraY = targetY + HOOD_CAM_HEIGHT;
        desiredCameraZ = targetZ + (std::cos(targetRotation) * HOOD_CAM_FORWARD_OFFSET);

        // Look ahead in the direction vehicle is facing
        desiredLookAtX = targetX + (std::sin(targetRotation) * HOOD_CAM_LOOK_DISTANCE);
        desiredLookAtY = targetY + HOOD_CAM_HEIGHT;
        desiredLookAtZ = targetZ + (std::cos(targetRotation) * HOOD_CAM_LOOK_DISTANCE);
    } else {
        // Follow cam - position camera behind vehicle
        desiredCameraX = targetX - (std::sin(targetRotation) * cameraDistance_);
        desiredCameraY = targetY + cameraHeight_;
        desiredCameraZ = targetZ - (std::cos(targetRotation) * cameraDistance_);

        // Look at the vehicle
        desiredLookAtX = targetX;
        desiredLookAtY = targetY;
        desiredLookAtZ = targetZ;
    }

    // Smoothly interpolate camera position
    currentCameraX_ += (desiredCameraX - currentCameraX_) * cameraLerpSpeed_;
    currentCameraY_ += (desiredCameraY - currentCameraY_) * cameraLerpSpeed_;
    currentCameraZ_ += (desiredCameraZ - currentCameraZ_) * cameraLerpSpeed_;

    // Smoothly interpolate look-at target
    currentLookAtX_ += (desiredLookAtX - currentLookAtX_) * cameraLerpSpeed_;
    currentLookAtY_ += (desiredLookAtY - currentLookAtY_) * cameraLerpSpeed_;
    currentLookAtZ_ += (desiredLookAtZ - currentLookAtZ_) * cameraLerpSpeed_;

    // Apply smoothed position and look-at
    camera_->position.set(currentCameraX_, currentCameraY_, currentCameraZ_);
    camera_->lookAt(Vector3(currentLookAtX_, currentLookAtY_, currentLookAtZ_));
}

void SceneManager::updateMinimapCamera(float targetX, float targetZ) {
    // Keep minimap centered on vehicle
    minimapCamera_->position.set(targetX, MINIMAP_HEIGHT, targetZ);
    minimapCamera_->lookAt(Vector3(targetX, 0, targetZ));
}

void SceneManager::updateCameraFOV(bool nitrousActive) {
    // Set target FOV based on nitrous state
    targetFOV_ = nitrousActive ? NITROUS_FOV : baseFOV_;

    // Smoothly interpolate current FOV to target
    currentFOV_ += (targetFOV_ - currentFOV_) * fovLerpSpeed_;

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
        -MINIMAP_VIEW_SIZE * aspectRatio, MINIMAP_VIEW_SIZE * aspectRatio,
        MINIMAP_VIEW_SIZE, -MINIMAP_VIEW_SIZE,
        CAMERA_NEAR, CAMERA_FAR
    );

    // Position camera above scene looking down
    minimapCamera_->position.set(0, MINIMAP_HEIGHT, 0);
    minimapCamera_->lookAt(Vector3(0, 0, 0));
}

void SceneManager::setCameraMode(CameraMode mode) {
    cameraMode_ = mode;
}

CameraMode SceneManager::getCameraMode() const {
    return cameraMode_;
}

void SceneManager::toggleCameraMode() {
    cameraMode_ = (cameraMode_ == CameraMode::FOLLOW) ? CameraMode::HOOD : CameraMode::FOLLOW;
}

void SceneManager::renderMinimap() {
    renderer_->render(*scene_, *minimapCamera_);
}
