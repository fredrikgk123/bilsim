#include "sceneManager.hpp"
#include <cmath>

using namespace threepp;

SceneManager::SceneManager()
    : cameraDistance_(8.0f),
      cameraHeight_(4.0f),
      cameraLerpSpeed_(0.1f),  // Change smoothnes of followcam
      currentCameraX_(0.0f),
      currentCameraY_(4.0f),
      currentCameraZ_(8.0f),
      currentLookAtX_(0.0f),
      currentLookAtY_(0.0f),
      currentLookAtZ_(0.0f) {
    scene_ = std::make_shared<Scene>();
    renderer_ = std::make_unique<GLRenderer>();

    // Enable performance optimizations
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
    auto ambientLight = AmbientLight::create(0x404040);
    scene_->add(ambientLight);
    
    auto directionalLight = DirectionalLight::create(0xffffff, 0.8f);
    directionalLight->position.set(5, 10, 7);
    directionalLight->castShadow = true;
    scene_->add(directionalLight);
}

void SceneManager::setupGround() {
    auto groundGeometry = PlaneGeometry::create(50, 50);
    auto groundMaterial = MeshPhongMaterial::create();
    groundMaterial->color = Color::gray;
    
    groundMesh_ = Mesh::create(groundGeometry, groundMaterial);
    groundMesh_->rotation.x = -math::PI / 2;
    groundMesh_->receiveShadow = true;
    scene_->add(groundMesh_);
}

void SceneManager::setupCamera(float aspectRatio) {
    camera_ = std::make_shared<PerspectiveCamera>(75, aspectRatio, 0.1f, 1000.0f);
    camera_->position.set(currentCameraX_, currentCameraY_, currentCameraZ_);
    camera_->lookAt(Vector3(currentLookAtX_, currentLookAtY_, currentLookAtZ_));
}

void SceneManager::updateCameraFollowTarget(float targetX, float targetY, float targetZ, float targetRotation) {
    // Calculate desired camera position behind and above the vehicle
    float desiredCameraX = targetX - std::sin(targetRotation) * cameraDistance_;
    float desiredCameraY = targetY + cameraHeight_;
    float desiredCameraZ = targetZ - std::cos(targetRotation) * cameraDistance_;

    // Smoothly interpolate camera position (lerp)
    currentCameraX_ += (desiredCameraX - currentCameraX_) * cameraLerpSpeed_;
    currentCameraY_ += (desiredCameraY - currentCameraY_) * cameraLerpSpeed_;
    currentCameraZ_ += (desiredCameraZ - currentCameraZ_) * cameraLerpSpeed_;

    // Smoothly interpolate look-at target
    currentLookAtX_ += (targetX - currentLookAtX_) * cameraLerpSpeed_;
    currentLookAtY_ += (targetY - currentLookAtY_) * cameraLerpSpeed_;
    currentLookAtZ_ += (targetZ - currentLookAtZ_) * cameraLerpSpeed_;

    // Apply smoothed position and rotation
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
