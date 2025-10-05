#include "sceneManager.hpp"
#include <cmath>

using namespace threepp;

SceneManager::SceneManager()
    : cameraDistance_(8.0f),       // Distance behind vehicle - far enough to see surroundings
      cameraHeight_(4.0f),         // Height above vehicle - provides good viewing angle
      cameraLerpSpeed_(0.1f),      // Smoothing factor (0.1 = smooth, 1.0 = instant) - creates cinematic camera movement
      currentCameraX_(0.0f),
      currentCameraY_(4.0f),
      currentCameraZ_(8.0f),
      currentLookAtX_(0.0f),
      currentLookAtY_(0.0f),
      currentLookAtZ_(0.0f) {
    scene_ = std::make_shared<Scene>();
    renderer_ = std::make_unique<GLRenderer>();

    // Enable shadows for more realistic rendering
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
    // Ambient light provides base illumination (prevents completely black shadows)
    auto ambientLight = AmbientLight::create(0x404040);  // Dim gray - subtle base lighting
    scene_->add(ambientLight);
    
    // Directional light simulates sun/main light source
    auto directionalLight = DirectionalLight::create(0xffffff, 0.8f);  // White light at 80% intensity
    directionalLight->position.set(5, 10, 7);  // Positioned above and to the side for natural shadows
    directionalLight->castShadow = true;

    // Configure shadow camera to match ground plane size (50x50)
    // This ensures shadows are rendered across the entire visible area
    auto shadowCamera = directionalLight->shadow->camera->as<OrthographicCamera>();
    shadowCamera->left = -25;
    shadowCamera->right = 25;
    shadowCamera->top = 25;
    shadowCamera->bottom = -25;
    shadowCamera->updateProjectionMatrix();

    scene_->add(directionalLight);
}

void SceneManager::setupGround() {
    auto groundGeometry = PlaneGeometry::create(50, 50);  // 50x50 unit ground plane
    auto groundMaterial = MeshPhongMaterial::create();
    groundMaterial->color = Color::gray;
    
    groundMesh_ = Mesh::create(groundGeometry, groundMaterial);
    groundMesh_->rotation.x = -math::PI / 2;  // Rotate to horizontal (XZ plane)
    groundMesh_->receiveShadow = true;
    scene_->add(groundMesh_);

    // Add grid helper for visual reference
    auto grid = GridHelper::create(50, 50);  // Size 50, divided into 50 segments (1 unit per square)
    grid->position.y = 0.01f;  // Slightly above ground to avoid z-fighting (flickering)
    scene_->add(grid);
}

void SceneManager::setupCamera(float aspectRatio) {
    // Field of view: 75° - wide enough for good spatial awareness
    // Near/far planes: 0.1 to 1000 - handles objects very close and very far
    camera_ = std::make_shared<PerspectiveCamera>(75, aspectRatio, 0.1f, 1000.0f);
    camera_->position.set(currentCameraX_, currentCameraY_, currentCameraZ_);
    camera_->lookAt(Vector3(currentLookAtX_, currentLookAtY_, currentLookAtZ_));
}

void SceneManager::setupRenderer(const WindowSize& size) {
    renderer_->setSize(size);
    renderer_->setClearColor(Color::aliceblue);
}

void SceneManager::updateCameraFollowTarget(float targetX, float targetY, float targetZ, float targetRotation) {
    // Calculate desired camera position behind and above the vehicle
    // Uses vehicle's rotation to stay behind it as it turns
    float desiredCameraX = targetX - std::sin(targetRotation) * cameraDistance_;
    float desiredCameraY = targetY + cameraHeight_;
    float desiredCameraZ = targetZ - std::cos(targetRotation) * cameraDistance_;

    // Smoothly interpolate camera position (lerp) - prevents jarring camera movements
    // Formula: current += (target - current) * lerpSpeed
    currentCameraX_ += (desiredCameraX - currentCameraX_) * cameraLerpSpeed_;
    currentCameraY_ += (desiredCameraY - currentCameraY_) * cameraLerpSpeed_;
    currentCameraZ_ += (desiredCameraZ - currentCameraZ_) * cameraLerpSpeed_;

    // Smoothly interpolate look-at target (camera tracks vehicle smoothly)
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
