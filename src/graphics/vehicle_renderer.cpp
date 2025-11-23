// AI Assistance: GitHub Copilot was used for OBJ file loading,
// 3D model transformation matrices, and wheel rotation calculations.

#include "graphics/vehicle_renderer.hpp"
#include <threepp/loaders/OBJLoader.hpp>
#include <algorithm>
#include <cmath>

using namespace threepp;

namespace {
    // Wheel model filenames relative to assets/
    const std::string WHEELS_DIR = "assets/wheels/";
    const std::string WHEEL_FL = "FLwheel.obj";
    const std::string WHEEL_FR = "FRwheel.obj";
    const std::string WHEEL_RL = "RLwheel.obj";
    const std::string WHEEL_RR = "RRwheel.obj";

    // Steering wheel configuration
    const std::string STEERING_WHEEL_PATH = "assets/steeringwheel.obj";


    // Wheel positioning: inset from vehicle edges to place wheels under fenders
    constexpr float WHEEL_INSET_X = 0.5f;
    constexpr float WHEEL_INSET_Z_FRONT = 1.0f;
    constexpr float WHEEL_INSET_Z_REAR = 1.0f;

    // Wheel animation tuning
    constexpr float WHEEL_RADIUS_FACTOR = 0.18f;
    constexpr float MAX_WHEEL_STEER_ANGLE = 0.5f;
    constexpr float MAX_BODY_SCALE_UP = 3.f;

    // Compute per-axis scale factors to make the placeholder body match the visible
    // dimensions of a loaded model. If no custom model is present return 1,1,1.
    std::array<float, 3> computeBodyScaleFromModel(const std::shared_ptr<threepp::Object3D>& modelGroup, const std::array<float,3>& vehicleSize) {
        if (!modelGroup) return {1.f, 1.f, 1.f};
        threepp::Box3 bbox;
        bbox.setFromObject(*modelGroup);
        auto modelSize = bbox.getSize();

        float sx = vehicleSize[0] > 0.f ? (modelSize.x / vehicleSize[0]) : 1.f;
        float sy = vehicleSize[1] > 0.f ? (modelSize.y / vehicleSize[1]) : 1.f;
        float sz = vehicleSize[2] > 0.f ? (modelSize.z / vehicleSize[2]) : 1.f;

        sx = (std::max)(1.f, sx);
        sy = (std::max)(1.f, sy);
        sz = (std::max)(1.f, sz);

        sx = (std::min)(sx, MAX_BODY_SCALE_UP);
        sy = (std::min)(sy, MAX_BODY_SCALE_UP);
        sz = (std::min)(sz, MAX_BODY_SCALE_UP);

        return {sx, sy, sz};
    }
}

VehicleRenderer::VehicleRenderer(Scene& scene, const IVehicleState& vehicleState)
    : GameObjectRenderer(scene, dynamic_cast<const GameObject&>(vehicleState)),
      vehicleState_(vehicleState),
      useCustomModel_(false),
      customModelGroup_(nullptr),
      modelScale_(1.f),
      steeringWheel_(nullptr),
      wheelFL_(nullptr),
      wheelFR_(nullptr),
      wheelRL_(nullptr),
      wheelRR_(nullptr) {
    GameObjectRenderer::createModel();

    if (bodyMesh_) {
        if (auto material = std::dynamic_pointer_cast<MeshPhongMaterial>(bodyMesh_->material())) {
            material->color = Color::red;
        }
        auto size = gameObject_.getSize();
        bodyMesh_->position.y = size[1] / 2.f;
        bodyMesh_->castShadow = true;
    }

    loadWheelModels(WHEELS_DIR);
    loadSteeringWheel(STEERING_WHEEL_PATH);

    actualAppliedScale_ = modelScale_;
    applyWheelScaleAndPosition(actualAppliedScale_);
    applySteeringWheelScaleAndPosition(actualAppliedScale_);
}

bool VehicleRenderer::loadModel(const std::string& modelPath) {
    try {
        OBJLoader loader;
        auto loadedGroup = loader.load(modelPath);

        if (!loadedGroup) {
            return false;
        }

        // Clear existing model
        if (bodyMesh_) {
            objectGroup_->remove(*bodyMesh_);
            bodyMesh_.reset();
        }

        // Compute the model's original bounding box to determine proper scaling
        Box3 modelBBox;
        modelBBox.setFromObject(*loadedGroup);
        auto originalModelSize = modelBBox.getSize();

        // Get the target vehicle dimensions
        auto vehicleSize = gameObject_.getSize();

        // Calculate scale factors for each axis to match vehicle dimensions
        float scaleX = vehicleSize[0] / originalModelSize.x;
        float scaleY = vehicleSize[1] / originalModelSize.y;
        float scaleZ = vehicleSize[2] / originalModelSize.z;

        // Use the average scale to maintain proportions, or use the smallest
        // to ensure the model fits within the collision box
        float autoScale = (std::min)({scaleX, scaleY, scaleZ});

        // Apply both the calculated auto-scale and any user-defined modelScale
        float appliedScale = autoScale * modelScale_;

        // Store the actual applied scale for wheel positioning
        actualAppliedScale_ = appliedScale;

        loadedGroup->scale.setScalar(appliedScale);
        loadedGroup->position.y = 0.f;

        // Enable shadows for all meshes
        loadedGroup->traverse([](Object3D& obj) {
            if (auto mesh = obj.as<Mesh>()) {
                mesh->castShadow = true;
                mesh->receiveShadow = false;
            }
        });

        objectGroup_->add(loadedGroup);
        customModelGroup_ = loadedGroup;
        useCustomModel_ = true;

        // Hide the fallback box mesh when using custom model
        if (bodyMesh_) {
            bodyMesh_->visible = false;
        }

        loadWheelModels(WHEELS_DIR);
        loadSteeringWheel(STEERING_WHEEL_PATH);

        // Apply wheel scale/positions for the current vehicle size
        applyWheelScaleAndPosition(appliedScale);
        applySteeringWheelScaleAndPosition(appliedScale);


        return true;

    } catch (const std::exception&) {
        return false;
    }
}

void VehicleRenderer::unloadModel() {
    // Remove any custom loaded model and fall back to the box mesh
    if (customModelGroup_) {
        objectGroup_->remove(*customModelGroup_);
        customModelGroup_.reset();
    }
    useCustomModel_ = false;

    // Show the fallback box mesh again
    if (bodyMesh_) {
        bodyMesh_->visible = true;
    }

    // Remove wheel models if present
    unloadWheelModels();

    // Remove steering wheel if present
    unloadSteeringWheel();
}

void VehicleRenderer::applyScale(float scale) {
    modelScale_ = scale;

    if (useCustomModel_ && customModelGroup_) {
        // When we have a custom model, we need to recalculate auto-scale
        // Get the original model size
        Box3 modelBBox;
        modelBBox.setFromObject(*customModelGroup_);
        auto originalModelSize = modelBBox.getSize();

        // Get the target vehicle dimensions
        auto vehicleSize = gameObject_.getSize();

        // Recalculate scale factors
        float scaleX = vehicleSize[0] / originalModelSize.x;
        float scaleY = vehicleSize[1] / originalModelSize.y;
        float scaleZ = vehicleSize[2] / originalModelSize.z;
        float autoScale = (std::min)({scaleX, scaleY, scaleZ});

        // Apply both auto-scale and user scale
        float appliedScale = autoScale * modelScale_;
        actualAppliedScale_ = appliedScale;

        customModelGroup_->scale.setScalar(appliedScale);
        customModelGroup_->position.y = 0.f;

        // Also scale and position wheels
        applyWheelScaleAndPosition(appliedScale);
        applySteeringWheelScaleAndPosition(appliedScale);

        if (bodyMesh_) {
            auto size = gameObject_.getSize();
            auto scales = computeBodyScaleFromModel(customModelGroup_, size);
            bodyMesh_->scale.set(scales[0], scales[1], scales[2]);
            bodyMesh_->position.y = (size[1] * scales[1]) / 2.f;
        }
    } else {
        actualAppliedScale_ = modelScale_;

        if (bodyMesh_) {
            objectGroup_->remove(*bodyMesh_);
            bodyMesh_.reset();
        }
        createModel();
    }
}

void VehicleRenderer::createModel() {
    std::array<float, 3> size = gameObject_.getSize();

    auto geometry = BoxGeometry::create(size[0], size[1], size[2]);
    auto material = MeshPhongMaterial::create();
    material->color = Color::red;

    bodyMesh_ = Mesh::create(geometry, material);

    auto scales = computeBodyScaleFromModel(customModelGroup_, size);
    bodyMesh_->scale.set(scales[0], scales[1], scales[2]);

    bodyMesh_->position.y = (size[1] * scales[1]) / 2.f;
    bodyMesh_->castShadow = true;

    objectGroup_->add(bodyMesh_);

    loadWheelModels(WHEELS_DIR);
    loadSteeringWheel(STEERING_WHEEL_PATH);

    float appliedScale = modelScale_;
    actualAppliedScale_ = appliedScale;
    applyWheelScaleAndPosition(appliedScale);
    applySteeringWheelScaleAndPosition(appliedScale);
}

// Helper: load wheel OBJ files from assets/wheels directory
bool VehicleRenderer::loadWheelModels(const std::string& wheelsDir) {
    try {
        OBJLoader loader;
        bool allLoaded = true;

        // Front-left
        if (!wheelFL_) {
            auto g = loader.load(wheelsDir + WHEEL_FL);
            if (g) {
                // Compute geometry center (bounding box center) and store it; we'll offset children when reparenting
                threepp::Box3 bbox;
                bbox.setFromObject(*g);
                auto center = bbox.getCenter();
                wheelCenterFL_ = {center.x, center.y, center.z};

                // Detect spin axis: the smallest bbox extent is the wheel thickness axis -> spin axis
                {
                    auto size = bbox.getSize();
                    float sx = size.x, sy = size.y, sz = size.z;
                    if (sx <= sy && sx <= sz) wheelSpinAxisFL_ = WheelSpinAxis::X;
                    else if (sy <= sx && sy <= sz) wheelSpinAxisFL_ = WheelSpinAxis::Y;
                    else wheelSpinAxisFL_ = WheelSpinAxis::Z;
                }

                // Instead of changing geometry, set the loaded group's local offset so its center becomes origin for spin
                g->position.set(-center.x, -center.y, -center.z);

                // Create pivot group and a dedicated spin group that will be rotated to spin the wheel
                wheelFLPivot_ = std::make_shared<Group>();
                auto spinFL = std::make_shared<Group>();
                // g has been offset so its center is at the spin group's origin
                spinFL->add(g);
                wheelFL_ = spinFL;
                wheelFL_->position.set(0.0f, 0.0f, 0.0f);
                wheelFLPivot_->add(wheelFL_);
                // Parent the pivot to the vehicle's objectGroup_ (consistent coordinate space)
                objectGroup_->add(wheelFLPivot_);
                wheelFLPivotOnCustom_ = false;
            } else {
                allLoaded = false;
            }
        }

        // Front-right
        if (!wheelFR_) {
            auto g = loader.load(wheelsDir + WHEEL_FR);
            if (g) {
                threepp::Box3 bbox;
                bbox.setFromObject(*g);
                auto center = bbox.getCenter();
                wheelCenterFR_ = {center.x, center.y, center.z};

                // Detect spin axis for front-right
                {
                    auto size = bbox.getSize();
                    float sx = size.x, sy = size.y, sz = size.z;
                    if (sx <= sy && sx <= sz) wheelSpinAxisFR_ = WheelSpinAxis::X;
                    else if (sy <= sx && sy <= sz) wheelSpinAxisFR_ = WheelSpinAxis::Y;
                    else wheelSpinAxisFR_ = WheelSpinAxis::Z;
                }

                g->position.set(-center.x, -center.y, -center.z);

                wheelFRPivot_ = std::make_shared<Group>();
                auto spinFR = std::make_shared<Group>();
                spinFR->add(g);
                wheelFR_ = spinFR;
                wheelFR_->position.set(0, 0, 0);
                wheelFRPivot_->add(wheelFR_);
                objectGroup_->add(wheelFRPivot_);
                wheelFRPivotOnCustom_ = false;
            } else {
                allLoaded = false;
            }
        }

        // Rear-left
        if (!wheelRL_) {
            auto g = loader.load(wheelsDir + WHEEL_RL);
            if (g) {
                threepp::Box3 bbox;
                bbox.setFromObject(*g);
                auto center = bbox.getCenter();
                wheelCenterRL_ = {center.x, center.y, center.z};

                // Detect spin axis for rear-left
                {
                    auto size = bbox.getSize();
                    float sx = size.x, sy = size.y, sz = size.z;
                    if (sx <= sy && sx <= sz) wheelSpinAxisRL_ = WheelSpinAxis::X;
                    else if (sy <= sx && sy <= sz) wheelSpinAxisRL_ = WheelSpinAxis::Y;
                    else wheelSpinAxisRL_ = WheelSpinAxis::Z;
                }

                g->position.set(-center.x, -center.y, -center.z);

                wheelRLPivot_ = std::make_shared<Group>();
                auto spinRL = std::make_shared<Group>();
                spinRL->add(g);
                wheelRL_ = spinRL;
                wheelRL_->position.set(0, 0, 0);
                wheelRLPivot_->add(wheelRL_);
                objectGroup_->add(wheelRLPivot_);
                wheelRLPivotOnCustom_ = false;
            } else {
                allLoaded = false;
            }
        }

        // Rear-right
        if (!wheelRR_) {
            auto g = loader.load(wheelsDir + WHEEL_RR);
            if (g) {
                threepp::Box3 bbox;
                bbox.setFromObject(*g);
                auto center = bbox.getCenter();
                wheelCenterRR_ = {center.x, center.y, center.z};

                // Detect spin axis for rear-right
                {
                    auto size = bbox.getSize();
                    float sx = size.x, sy = size.y, sz = size.z;
                    if (sx <= sy && sx <= sz) wheelSpinAxisRR_ = WheelSpinAxis::X;
                    else if (sy <= sx && sy <= sz) wheelSpinAxisRR_ = WheelSpinAxis::Y;
                    else wheelSpinAxisRR_ = WheelSpinAxis::Z;
                }

                g->position.set(-center.x, -center.y, -center.z);

                wheelRRPivot_ = std::make_shared<Group>();
                auto spinRR = std::make_shared<Group>();
                spinRR->add(g);
                wheelRR_ = spinRR;
                wheelRR_->position.set(0, 0, 0);
                wheelRRPivot_->add(wheelRR_);
                objectGroup_->add(wheelRRPivot_);
                wheelRRPivotOnCustom_ = false;
            } else {
                allLoaded = false;
            }
        }

        return allLoaded;
    } catch (const std::exception&) {
        return false;
    }
}

void VehicleRenderer::unloadWheelModels() {
    // Remove pivot groups (which contain the wheel meshes)
    if (wheelFLPivot_) {
        if (wheelFLPivotOnCustom_ && customModelGroup_) customModelGroup_->remove(*wheelFLPivot_);
        else objectGroup_->remove(*wheelFLPivot_);
        wheelFLPivot_.reset();
        wheelFLPivotOnCustom_ = false;
    }
    if (wheelFRPivot_) {
        if (wheelFRPivotOnCustom_ && customModelGroup_) customModelGroup_->remove(*wheelFRPivot_);
        else objectGroup_->remove(*wheelFRPivot_);
        wheelFRPivot_.reset();
        wheelFRPivotOnCustom_ = false;
    }
    if (wheelRLPivot_) {
        if (wheelRLPivotOnCustom_ && customModelGroup_) customModelGroup_->remove(*wheelRLPivot_);
        else objectGroup_->remove(*wheelRLPivot_);
        wheelRLPivot_.reset();
        wheelRLPivotOnCustom_ = false;
    }
    if (wheelRRPivot_) {
        if (wheelRRPivotOnCustom_ && customModelGroup_) customModelGroup_->remove(*wheelRRPivot_);
        else objectGroup_->remove(*wheelRRPivot_);
        wheelRRPivot_.reset();
        wheelRRPivotOnCustom_ = false;
    }

    // Reset wheel pointers as well
    wheelFL_.reset();
    wheelFR_.reset();
    wheelRL_.reset();
    wheelRR_.reset();
}

// Helper: load steering wheel OBJ file
bool VehicleRenderer::loadSteeringWheel(const std::string& steeringWheelPath) {
    try {
        OBJLoader loader;

        if (!steeringWheel_) {
            auto g = loader.load(steeringWheelPath);
            if (g) {
                // Compute geometry center (bounding box center) and store it
                threepp::Box3 bbox;
                bbox.setFromObject(*g);
                auto center = bbox.getCenter();
                steeringWheelCenter_ = {center.x, center.y, center.z};

                // Detect rotation axis: the steering wheel's smallest dimension is likely the thickness
                // The rotation axis should be perpendicular to the wheel's face (the thinnest dimension)
                {
                    auto size = bbox.getSize();
                    float sx = size.x, sy = size.y, sz = size.z;
                    if (sx <= sy && sx <= sz) steeringWheelRotationAxis_ = WheelSpinAxis::X;
                    else if (sy <= sx && sy <= sz) steeringWheelRotationAxis_ = WheelSpinAxis::Y;
                    else steeringWheelRotationAxis_ = WheelSpinAxis::Z;
                }

                // Offset the loaded group so its center becomes the origin for rotation
                g->position.set(-center.x, -center.y, -center.z);

                // Create pivot group and a spin group that will be rotated for steering
                steeringWheelPivot_ = std::make_shared<Group>();
                auto spinGroup = std::make_shared<Group>();
                // g has been offset so its center is at the spin group's origin
                spinGroup->add(g);
                steeringWheel_ = spinGroup;
                steeringWheel_->position.set(0.0f, 0.0f, 0.0f);
                steeringWheelPivot_->add(steeringWheel_);
                objectGroup_->add(steeringWheelPivot_);
                steeringWheelPivotOnCustom_ = false;

                return true;
            } else {
                return false;
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void VehicleRenderer::unloadSteeringWheel() {
    if (steeringWheelPivot_) {
        if (steeringWheelPivotOnCustom_ && customModelGroup_) {
            customModelGroup_->remove(*steeringWheelPivot_);
        } else {
            objectGroup_->remove(*steeringWheelPivot_);
        }
        steeringWheelPivot_.reset();
        steeringWheelPivotOnCustom_ = false;
    }
    steeringWheel_.reset();
}

void VehicleRenderer::applySteeringWheelScaleAndPosition(float appliedScale) {
    if (!steeringWheelPivot_ || !steeringWheel_) return;

    // Calculate proper position including the center offset
    float wheelScale = appliedScale;
    float px = steeringWheelCenter_[0] * wheelScale;
    float py = steeringWheelCenter_[1] * wheelScale;
    float pz = steeringWheelCenter_[2] * wheelScale;

    // Position the steering wheel pivot in vehicle-local coordinates
    steeringWheelPivot_->position.set(px, py, pz);
    steeringWheelPivot_->rotation.set(0, 0, 0);

    // Keep steering wheel child at origin so it rotates around its own center
    steeringWheel_->scale.setScalar(wheelScale);
    steeringWheel_->position.set(0, 0, 0);

    steeringWheel_->traverse([](Object3D& obj) {
        if (auto mesh = obj.as<Mesh>()) {
            mesh->castShadow = true;
            mesh->receiveShadow = false;
        }
    });
}

void VehicleRenderer::applyWheelScaleAndPosition(float appliedScale) {
    std::array<float, 3> size = gameObject_.getSize();

    float halfWidth = size[0] * 0.5f;
    float halfLength = size[2] * 0.5f;

    float xInset = WHEEL_INSET_X;
    float zFrontInset = WHEEL_INSET_Z_FRONT;
    float zRearInset = WHEEL_INSET_Z_REAR;

    float wheelCenterY = 0.f;
    float wheelScale = appliedScale;

    if (wheelFLPivot_) {
        float pxFL = (-halfWidth + xInset) + (wheelCenterFL_[0] * wheelScale);
        float pyFL = wheelCenterY + (wheelCenterFL_[1] * wheelScale);
        float pzFL = (halfLength - zFrontInset) + (wheelCenterFL_[2] * wheelScale);
        wheelFLPivot_->position.set(pxFL, pyFL, pzFL);
        wheelFLPivot_->rotation.set(0, 0, 0);
        if (wheelFL_) {
            wheelFL_->scale.setScalar(wheelScale);
            wheelFL_->position.set(0, 0, 0);
            wheelFL_->traverse([](Object3D& obj) {
                if (auto mesh = obj.as<Mesh>()) {
                    mesh->castShadow = true;
                    mesh->receiveShadow = false;
                }
            });
        }
    }

    if (wheelFRPivot_) {
        float pxFR = (halfWidth - xInset) + (wheelCenterFR_[0] * wheelScale);
        float pyFR = wheelCenterY + (wheelCenterFR_[1] * wheelScale);
        float pzFR = (halfLength - zFrontInset) + (wheelCenterFR_[2] * wheelScale);
        wheelFRPivot_->position.set(pxFR, pyFR, pzFR);
        wheelFRPivot_->rotation.set(0, 0, 0);
        if (wheelFR_) {
            wheelFR_->scale.setScalar(wheelScale);
            wheelFR_->position.set(0, 0, 0);
            wheelFR_->traverse([](Object3D& obj) {
                if (auto mesh = obj.as<Mesh>()) {
                    mesh->castShadow = true;
                    mesh->receiveShadow = false;
                }
            });
        }
    }

    if (wheelRLPivot_) {
        float pxRL = (-halfWidth + xInset) + (wheelCenterRL_[0] * wheelScale);
        float pyRL = wheelCenterY + (wheelCenterRL_[1] * wheelScale);
        float pzRL = (-halfLength + zRearInset) + (wheelCenterRL_[2] * wheelScale);
        wheelRLPivot_->position.set(pxRL, pyRL, pzRL);
        wheelRLPivot_->rotation.set(0, 0, 0);
        if (wheelRL_) {
            wheelRL_->scale.setScalar(wheelScale);
            wheelRL_->position.set(0, 0, 0);
            wheelRL_->traverse([](Object3D& obj) {
                if (auto mesh = obj.as<Mesh>()) {
                    mesh->castShadow = true;
                    mesh->receiveShadow = false;
                }
            });
        }
    }

    if (wheelRRPivot_) {
        float pxRR = (halfWidth - xInset) + (wheelCenterRR_[0] * wheelScale);
        float pyRR = wheelCenterY + (wheelCenterRR_[1] * wheelScale);
        float pzRR = (-halfLength + zRearInset) + (wheelCenterRR_[2] * wheelScale);
        wheelRRPivot_->position.set(pxRR, pyRR, pzRR);
        wheelRRPivot_->rotation.set(0, 0, 0);
        if (wheelRR_) {
            wheelRR_->scale.setScalar(wheelScale);
            wheelRR_->position.set(0, 0, 0);
            wheelRR_->traverse([](Object3D& obj) {
                if (auto mesh = obj.as<Mesh>()) {
                    mesh->castShadow = true;
                    mesh->receiveShadow = false;
                }
            });
        }
    }
}

// Update visual elements each frame: position/rotation already handled by base class,
// but here we also animate wheel spin (rotation around local axis) and front wheel yaw.
void VehicleRenderer::update(bool leftPressed, bool rightPressed) {
    GameObjectRenderer::update();

    auto pos = gameObject_.getPosition();

    float dx = pos[0] - prevPosition_[0];
    float dz = pos[2] - prevPosition_[2];
    float distance = std::sqrt(dx * dx + dz * dz);

    std::array<float, 3> size = gameObject_.getSize();
    float wheelRadius = (std::max)(0.001f, size[1] * actualAppliedScale_ * WHEEL_RADIUS_FACTOR);

    float spinDelta = distance / wheelRadius;

    float velocity = vehicleState_.getVelocity();
    float spinDir = (velocity >= 0.f) ? 1.f : -1.f;

    // Accumulate wheel spin based on distance traveled (dampened for visual effect)
    if (wheelFL_) wheelSpinFL_ += spinDir * spinDelta * 0.1f;
    if (wheelFR_) wheelSpinFR_ += spinDir * spinDelta * 0.1f;
    if (wheelRL_) wheelSpinRL_ += spinDir * spinDelta * 0.1f;
    if (wheelRR_) wheelSpinRR_ += spinDir * spinDelta * 0.1f;

    // Calculate front wheel steering based purely on keyboard input (independent of vehicle physics)
    // The wheels always turn the same way visually - physics handles the actual turning behavior
    float targetWheelSteerYaw = 0.f;

    if (leftPressed && !rightPressed) {
        // Turning left - rotate wheels left
        targetWheelSteerYaw = MAX_WHEEL_STEER_ANGLE;
    } else if (rightPressed && !leftPressed) {
        // Turning right - rotate wheels right
        targetWheelSteerYaw = -MAX_WHEEL_STEER_ANGLE;
    }
    // else: both or neither pressed = wheels straight (0)

    // Steering wheel rotation based purely on keyboard input (independent of vehicle physics)
    if (steeringWheelPivot_) {
        // Calculate target rotation based on raw key input
        float targetSteeringRotation = 0.f;

        if (leftPressed && !rightPressed) {
            // Turning left - rotate steering wheel counterclockwise
            targetSteeringRotation = -1.5f;  // ~90 degrees
        } else if (rightPressed && !leftPressed) {
            // Turning right - rotate steering wheel clockwise
            targetSteeringRotation = 1.5f;  // ~90 degrees
        }
        // else: both or neither pressed = return to center (0)

        // Smooth interpolation to target
        currentSteeringWheelRotation_ += (targetSteeringRotation - currentSteeringWheelRotation_) * 0.1f;

        // Apply rotation around the detected axis - this rotates the wheel like a real steering wheel
        switch (steeringWheelRotationAxis_) {
            case WheelSpinAxis::X: steeringWheelPivot_->rotation.x = currentSteeringWheelRotation_; break;
            case WheelSpinAxis::Y: steeringWheelPivot_->rotation.y = currentSteeringWheelRotation_; break;
            case WheelSpinAxis::Z: steeringWheelPivot_->rotation.z = currentSteeringWheelRotation_; break;
        }
    }

    if (wheelFLPivot_) {
        wheelFLPivot_->rotation.y = targetWheelSteerYaw;
        if (wheelFL_) {
            switch (wheelSpinAxisFL_) {
                case WheelSpinAxis::X: wheelFL_->rotation.x = wheelInvertFL_ * wheelSpinFL_; break;
                case WheelSpinAxis::Y: wheelFL_->rotation.y = wheelInvertFL_ * wheelSpinFL_; break;
                case WheelSpinAxis::Z: wheelFL_->rotation.z = wheelInvertFL_ * wheelSpinFL_; break;
            }
        }
    }
    if (wheelFRPivot_) {
        wheelFRPivot_->rotation.y = targetWheelSteerYaw;
        if (wheelFR_) {
            switch (wheelSpinAxisFR_) {
                case WheelSpinAxis::X: wheelFR_->rotation.x = wheelInvertFR_ * wheelSpinFR_; break;
                case WheelSpinAxis::Y: wheelFR_->rotation.y = wheelInvertFR_ * wheelSpinFR_; break;
                case WheelSpinAxis::Z: wheelFR_->rotation.z = wheelInvertFR_ * wheelSpinFR_; break;
            }
        }
    }
    if (wheelRLPivot_) {
        wheelRLPivot_->rotation.y = 0;
        if (wheelRL_) {
            switch (wheelSpinAxisRL_) {
                case WheelSpinAxis::X: wheelRL_->rotation.x = wheelInvertRL_ * wheelSpinRL_; break;
                case WheelSpinAxis::Y: wheelRL_->rotation.y = wheelInvertRL_ * wheelSpinRL_; break;
                case WheelSpinAxis::Z: wheelRL_->rotation.z = wheelInvertRL_ * wheelSpinRL_; break;
            }
        }
    }
    if (wheelRRPivot_) {
        wheelRRPivot_->rotation.y = 0;
        if (wheelRR_) {
            switch (wheelSpinAxisRR_) {
                case WheelSpinAxis::X: wheelRR_->rotation.x = wheelInvertRR_ * wheelSpinRR_; break;
                case WheelSpinAxis::Y: wheelRR_->rotation.y = wheelInvertRR_ * wheelSpinRR_; break;
                case WheelSpinAxis::Z: wheelRR_->rotation.z = wheelInvertRR_ * wheelSpinRR_; break;
            }
        }
    }

    prevPosition_ = pos;
}

std::array<float, 3> VehicleRenderer::getSteeringWheelPosition() const noexcept {
    if (steeringWheelPivot_) {
        auto pos = steeringWheelPivot_->position;
        return {pos.x, pos.y, pos.z};
    }
    // Default fallback position if no steering wheel
    return {0.f, 0.3f, 0.5f};
}

bool VehicleRenderer::hasSteeringWheel() const noexcept {
    return steeringWheelPivot_ != nullptr;
}
