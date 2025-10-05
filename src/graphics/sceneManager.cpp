#include "sceneManager.hpp"
#include <cmath>

using namespace threepp;

SceneManager::SceneManager()
    : cameraDistance_(8.0f),    // 8 units behind - far enough to see surroundings
      cameraHeight_(4.0f),      // 4 units above - provides good viewing angle without being too high
      cameraLerpSpeed_(0.1f),   // 0.1 = smooth, 1.0 = instant - creates cinematic camera movement
      currentCameraX_(0.0f),
      currentCameraY_(4.0f),
      currentCameraZ_(8.0f),
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
    auto ambientLight = AmbientLight::create(0x404040);  // 0x404040 = dim gray - prevents completely black shadows
    scene_->add(ambientLight);
    
    // Directional light for main lighting and shadows
    auto directionalLight = DirectionalLight::create(0xffffff, 0.8f);  // 0.8 = 80% intensity - bright but not blown out
    directionalLight->position.set(5, 10, 7);  // (5,10,7) - positioned above and to side for natural shadows
    directionalLight->castShadow = true;

    // Configure shadow camera
    auto shadowCamera = directionalLight->shadow->camera->as<OrthographicCamera>();
    shadowCamera->left = -25;    // Shadow area: 50x50 units matches ground size
    shadowCamera->right = 25;
    shadowCamera->top = 25;
    shadowCamera->bottom = -25;
    shadowCamera->updateProjectionMatrix();

    scene_->add(directionalLight);
}

void SceneManager::setupGround() {
    // Create ground plane
    auto groundGeometry = PlaneGeometry::create(50, 50);  // 50x50 units - large enough for driving around
    auto groundMaterial = MeshPhongMaterial::create();
    groundMaterial->color = Color::gray;
    
    groundMesh_ = Mesh::create(groundGeometry, groundMaterial);
    groundMesh_->rotation.x = -math::PI / 2;  // -90° - rotate from vertical (XY) to horizontal (XZ)
    groundMesh_->receiveShadow = true;
    scene_->add(groundMesh_);

    // Add grid helper for visual reference
    auto grid = GridHelper::create(50, 50);  // 50 divisions - 1 unit per grid square
    grid->position.y = 0.01f;  // 0.01 units above ground - prevents z-fighting flickering
    scene_->add(grid);
}

void SceneManager::setupCamera(float aspectRatio) {
    camera_ = std::make_shared<PerspectiveCamera>(75, aspectRatio, 0.1f, 1000.0f);  // FOV=75° (wide for awareness), near=0.1, far=1000
    camera_->position.set(currentCameraX_, currentCameraY_, currentCameraZ_);
    camera_->lookAt(Vector3(currentLookAtX_, currentLookAtY_, currentLookAtZ_));
}

void SceneManager::setupRenderer(const WindowSize& size) {
    renderer_->setSize(size);
    renderer_->setClearColor(Color::aliceblue);
}

void SceneManager::updateCameraFollowTarget(float targetX, float targetY, float targetZ, float targetRotation) {
    // Calculate desired camera position behind vehicle
    float desiredCameraX = targetX - std::sin(targetRotation) * cameraDistance_;
    float desiredCameraY = targetY + cameraHeight_;
    float desiredCameraZ = targetZ - std::cos(targetRotation) * cameraDistance_;

    // Smoothly interpolate camera position
    currentCameraX_ += (desiredCameraX - currentCameraX_) * cameraLerpSpeed_;
    currentCameraY_ += (desiredCameraY - currentCameraY_) * cameraLerpSpeed_;
    currentCameraZ_ += (desiredCameraZ - currentCameraZ_) * cameraLerpSpeed_;

    // Smoothly interpolate look-at target
    currentLookAtX_ += (targetX - currentLookAtX_) * cameraLerpSpeed_;
    currentLookAtY_ += (targetY - currentLookAtY_) * cameraLerpSpeed_;
    currentLookAtZ_ += (targetZ - currentLookAtZ_) * cameraLerpSpeed_;

    // Apply smoothed position and look-at
    camera_->position.set(currentCameraX_, currentCameraY_, currentCameraZ_);
    camera_->lookAt(Vector3(currentLookAtX_, currentLookAtY_, currentLookAtZ_));
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
    float viewSize = 15.0f;  // 15 units - covers 30x30 area, enough to see nearby surroundings
    minimapCamera_ = std::make_shared<OrthographicCamera>(
        -viewSize * aspectRatio, viewSize * aspectRatio,
        viewSize, -viewSize,
        0.1f, 1000.0f  // Near/far planes - standard range
    );

    // Position camera above scene looking down
    minimapCamera_->position.set(0, 50, 0);  // 50 units high - bird's eye view
    minimapCamera_->lookAt(Vector3(0, 0, 0));
}

void SceneManager::updateMinimapCamera(float targetX, float targetZ) {
    // Keep minimap centered on vehicle
    minimapCamera_->position.set(targetX, 50, targetZ);
    minimapCamera_->lookAt(Vector3(targetX, 0, targetZ));
}

void SceneManager::renderMinimap() {
    renderer_->render(*scene_, *minimapCamera_);
}
