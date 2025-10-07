#include "sceneManager.hpp"
#include <cmath>

using namespace threepp;

// Anonymous namespace - these constants are LOCAL to this file only (NOT global!)
namespace {
    // Camera constants
    const float DEFAULT_CAMERA_DISTANCE = 8.0f;   // 8 units behind - far enough to see surroundings
    const float DEFAULT_CAMERA_HEIGHT = 4.0f;     // 4 units above - provides good viewing angle
    const float DEFAULT_CAMERA_LERP_SPEED = 0.1f; // 0.1 = smooth, 1.0 = instant - creates cinematic camera movement
    const float MINIMAP_VIEW_SIZE = 15.0f;        // 15 units - covers 30x30 area
    const float MINIMAP_HEIGHT = 50.0f;           // 50 units high - bird's eye view

    // Ground/Grid constants
    const float GROUND_SIZE = 200.0f;             // 200x200 units - large play area
    const int GRID_DIVISIONS = GROUND_SIZE;       // Same as ground size - 1 unit per grid square
    const float GRID_Z_OFFSET = 0.01f;            // 0.01 units above ground - prevents z-fighting flickering

    // Camera constants
    const float CAMERA_FOV = 75.0f;               // FOV=75° (wide for awareness)
    const float NITROUS_FOV = 90.0f;              // FOV=90° during nitrous boost (wider = faster feel)
    const float FOV_LERP_SPEED = 0.05f;           // Smooth FOV transitions
    const float CAMERA_NEAR = 0.1f;
    const float CAMERA_FAR = 1000.0f;

    // Lighting constants
    const unsigned int AMBIENT_COLOR = 0x404040;  // Dim gray - prevents completely black shadows
    const float AMBIENT_INTENSITY = 1.0f;
    const unsigned int DIRECTIONAL_COLOR = 0xffffff;
    const float DIRECTIONAL_INTENSITY = 0.8f;     // 80% intensity - bright but not blown out
    const float SHADOW_AREA_SIZE = 200.0f;        // Shadow area: 400x400 units (covers entire play area)
}

SceneManager::SceneManager()
    : cameraDistance_(DEFAULT_CAMERA_DISTANCE),
      cameraHeight_(DEFAULT_CAMERA_HEIGHT),
      cameraLerpSpeed_(DEFAULT_CAMERA_LERP_SPEED),
      baseFOV_(CAMERA_FOV),
      currentFOV_(CAMERA_FOV),
      targetFOV_(CAMERA_FOV),
      fovLerpSpeed_(FOV_LERP_SPEED),
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

void SceneManager::updateCameraFollowTarget(float targetX, float targetY, float targetZ, float targetRotation) {
    // Calculate desired camera position behind vehicle
    float desiredCameraX = targetX - (std::sin(targetRotation) * cameraDistance_);
    float desiredCameraY = targetY + cameraHeight_;
    float desiredCameraZ = targetZ - (std::cos(targetRotation) * cameraDistance_);

    // Smoothly interpolate camera position
    currentCameraX_ = currentCameraX_ + ((desiredCameraX - currentCameraX_) * cameraLerpSpeed_);
    currentCameraY_ = currentCameraY_ + ((desiredCameraY - currentCameraY_) * cameraLerpSpeed_);
    currentCameraZ_ = currentCameraZ_ + ((desiredCameraZ - currentCameraZ_) * cameraLerpSpeed_);

    // Smoothly interpolate look-at target
    currentLookAtX_ = currentLookAtX_ + ((targetX - currentLookAtX_) * cameraLerpSpeed_);
    currentLookAtY_ = currentLookAtY_ + ((targetY - currentLookAtY_) * cameraLerpSpeed_);
    currentLookAtZ_ = currentLookAtZ_ + ((targetZ - currentLookAtZ_) * cameraLerpSpeed_);

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
    currentFOV_ = currentFOV_ + ((targetFOV_ - currentFOV_) * fovLerpSpeed_);

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

void SceneManager::renderMinimap() {
    renderer_->render(*scene_, *minimapCamera_);
}
