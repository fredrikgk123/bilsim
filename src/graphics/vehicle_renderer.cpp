#include "vehicle_renderer.hpp"
#include <threepp/loaders/OBJLoader.hpp>
#include <iostream>
#include <algorithm>

using namespace threepp;

namespace {
    constexpr float MODEL_SCALE = 0.5f;          // Scale factor for loaded 3D models
    constexpr float MODEL_VERTICAL_OFFSET = 0.5f; // Lift car above ground
    // Visual adjustment: lower the car a bit so it sits closer to the ground
    constexpr float CAR_LOWER_AMOUNT = 0.30f;    // lower car by 0.1 units
    // Additional small lift applied specifically to wheels
    constexpr float WHEEL_LIFT = 0.20f;         // raise wheels by 0.3 units
    // Wheel model filenames relative to assets/
    const std::string WHEELS_DIR = "assets/wheels/";
    const std::string WHEEL_FL = "wheel_fl.obj";
    const std::string WHEEL_FR = "wheel_fr.obj";
    const std::string WHEEL_RL = "wheel_rl.obj";
    const std::string WHEEL_RR = "wheel_rr.obj";
    // Tuning: reduce track width and wheelbase slightly so wheels sit a bit closer
    constexpr float WHEEL_TRACK_FACTOR = 0.98f; // Track width
    constexpr float WHEELBASE_FACTOR = 0.47f;   // Wheelset distance

    // Wheel animation tuning
    // Wheel radius is estimated as a factor of vehicle height (scaled by model scale)
    constexpr float WHEEL_RADIUS_FACTOR = 0.18f; // radius ~= VEHICLE_HEIGHT * appliedScale * factor
    // Steering visual tuning
    constexpr float STEERING_VISUAL_MULTIPLIER = 8.0f; // amplify rotation delta to look like steering
    constexpr float MAX_WHEEL_STEER_ANGLE = 0.50; // Radians

    // Safety cap for how much we scale the fallback body to match a model
    constexpr float MAX_BODY_SCALE_UP = 3.0f;

    // Compute per-axis scale factors to make the placeholder body match the visible
    // dimensions of a loaded model. If no custom model is present return 1,1,1.
    static std::array<float, 3> computeBodyScaleFromModel(const std::shared_ptr<threepp::Object3D>& modelGroup, const std::array<float,3>& vehicleSize) {
        if (!modelGroup) return {1.0f, 1.0f, 1.0f};
        threepp::Box3 bbox;
        bbox.setFromObject(*modelGroup);
        auto modelSize = bbox.getSize(); // Vector3

        // Avoid division by zero and compute per-axis ratios
        float sx = vehicleSize[0] > 0.0f ? (modelSize.x / vehicleSize[0]) : 1.0f;
        float sy = vehicleSize[1] > 0.0f ? (modelSize.y / vehicleSize[1]) : 1.0f;
        float sz = vehicleSize[2] > 0.0f ? (modelSize.z / vehicleSize[2]) : 1.0f;

        // We only scale up the placeholder (don't shrink below vehicle physics size)
        sx = std::max(1.0f, sx);
        sy = std::max(1.0f, sy);
        sz = std::max(1.0f, sz);

        // Clamp to a reasonable maximum to avoid runaway scaling from malformed models
        sx = std::min(sx, MAX_BODY_SCALE_UP);
        sy = std::min(sy, MAX_BODY_SCALE_UP);
        sz = std::min(sz, MAX_BODY_SCALE_UP);

        return {sx, sy, sz};
    }
}

VehicleRenderer::VehicleRenderer(Scene& scene, const Vehicle& vehicle)
    : GameObjectRenderer(scene, vehicle),
      vehicle_(vehicle),
      useCustomModel_(false),
      customModelGroup_(nullptr),
      modelScale_(1.0f),
      wheelFL_(nullptr),
      wheelFR_(nullptr),
      wheelRL_(nullptr),
      wheelRR_(nullptr) {
    // Call base class createModel to avoid virtual dispatch inside constructor
    GameObjectRenderer::createModel();

    // If the body mesh was created by base, adjust its appearance to match the vehicle renderer
    if (bodyMesh_) {
        if (auto material = std::dynamic_pointer_cast<MeshPhongMaterial>(bodyMesh_->material())) {
            material->color = Color::red; // Red for player vehicle
        }
        // Lower the fallback body slightly so it visually sits a bit closer to ground
        auto size = vehicle_.getSize();
        bodyMesh_->position.y = (size[1] / 2.0f) - CAR_LOWER_AMOUNT;
        bodyMesh_->castShadow = true;
    }

    // Try to load wheel models and position them
    if (!loadWheelModels(WHEELS_DIR)) {
        // If wheel models are unavailable, leave wheels empty (fallback)
    }

    float appliedScale = MODEL_SCALE * modelScale_;
    applyWheelScaleAndPosition(appliedScale);
}

bool VehicleRenderer::loadModel(const std::string& modelPath) {
    try {
        OBJLoader loader;
        auto loadedGroup = loader.load(modelPath);

        if (!loadedGroup) {
            std::cerr << "Failed to load model: " << modelPath << std::endl;
            return false;
        }

        // Clear existing model
        if (bodyMesh_) {
            objectGroup_->remove(*bodyMesh_);
            bodyMesh_.reset();
        }

        // Scale and position the loaded model
        float appliedScale = MODEL_SCALE * modelScale_;
        loadedGroup->scale.setScalar(appliedScale);
        // Apply a small downward adjustment so the model sits slightly lower
        loadedGroup->position.y = (MODEL_VERTICAL_OFFSET * appliedScale) - CAR_LOWER_AMOUNT;

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

        // Try to load wheel models and position them
        if (!loadWheelModels(WHEELS_DIR)) {
            std::cerr << "Warning: failed to load some wheel models from " << WHEELS_DIR << std::endl;
        }

        // Apply wheel scale/positions for the current vehicle size
        applyWheelScaleAndPosition(appliedScale);

        // Adjust the fallback body (if present) so it visually matches the loaded model.
        if (bodyMesh_) {
            auto size = vehicle_.getSize();
            auto scales = computeBodyScaleFromModel(customModelGroup_, size);
            // Apply per-axis scale to the body mesh
            bodyMesh_->scale.set(scales[0], scales[1], scales[2]);
            // Update body vertical offset based on scaled height
            bodyMesh_->position.y = ((size[1] * scales[1]) / 2.0f) - CAR_LOWER_AMOUNT;
        }

        std::cout << "Successfully loaded car model: " << modelPath << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
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

    // Remove wheel models if present
    unloadWheelModels();

    // Remove any existing body mesh so createModel can recreate it with current vehicle size
    if (bodyMesh_) {
        objectGroup_->remove(*bodyMesh_);
        bodyMesh_.reset();
    }

    createModel();
}

void VehicleRenderer::applyScale(float scale) {
    modelScale_ = scale;

    if (useCustomModel_ && customModelGroup_) {
        // Apply scale to loaded group
        float appliedScale = MODEL_SCALE * modelScale_;
        customModelGroup_->scale.setScalar(appliedScale);
        customModelGroup_->position.y = (MODEL_VERTICAL_OFFSET * appliedScale) - CAR_LOWER_AMOUNT;

        // Also scale and position wheels
        applyWheelScaleAndPosition(appliedScale);

        // If a fallback body exists, update it to visually match the (now rescaled) custom model
        if (bodyMesh_) {
            auto size = vehicle_.getSize();
            auto scales = computeBodyScaleFromModel(customModelGroup_, size);
            bodyMesh_->scale.set(scales[0], scales[1], scales[2]);
            bodyMesh_->position.y = ((size[1] * scales[1]) / 2.0f) - CAR_LOWER_AMOUNT;
        }
    } else {
        // Recreate fallback mesh with new size from vehicle
        if (bodyMesh_) {
            objectGroup_->remove(*bodyMesh_);
            bodyMesh_.reset();
        }
        createModel();
    }
}

void VehicleRenderer::createModel() {
    std::array<float, 3> size = vehicle_.getSize();

    // Create simple box geometry for vehicle (fallback)
    // The geometry is created using the physics size, but we may scale it visually to match
    // a loaded model's visible size (if present) so the placeholder better represents the mesh.
    auto geometry = BoxGeometry::create(size[0], size[1], size[2]);
    auto material = MeshPhongMaterial::create();
    material->color = Color::red;  // Red for player vehicle

    bodyMesh_ = Mesh::create(geometry, material);

    // If a custom model is present (e.g. we switched models without recreating the body),
    // compute per-axis scale factors so the placeholder matches the visible model size.
    auto scales = computeBodyScaleFromModel(customModelGroup_, size);
    bodyMesh_->scale.set(scales[0], scales[1], scales[2]);

    // Lower the fallback body slightly so it visually sits a bit closer to ground
    bodyMesh_->position.y = ((size[1] * scales[1]) / 2.0f) - CAR_LOWER_AMOUNT;
    bodyMesh_->castShadow = true;

    objectGroup_->add(bodyMesh_);

    // Load wheel models (if available) and position them for the fallback box
    if (!loadWheelModels(WHEELS_DIR)) {
        // If wheel models are unavailable, leave wheels empty (fallback)
        return;
    }

    float appliedScale = MODEL_SCALE * modelScale_;
    applyWheelScaleAndPosition(appliedScale);
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
                std::cerr << "Failed to load wheel: " << (wheelsDir + WHEEL_FL) << std::endl;
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
                std::cerr << "Failed to load wheel: " << (wheelsDir + WHEEL_FR) << std::endl;
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
                std::cerr << "Failed to load wheel: " << (wheelsDir + WHEEL_RL) << std::endl;
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
                std::cerr << "Failed to load wheel: " << (wheelsDir + WHEEL_RR) << std::endl;
                allLoaded = false;
            }
        }

        return allLoaded;
    } catch (const std::exception& e) {
        std::cerr << "Error loading wheel models: " << e.what() << std::endl;
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

void VehicleRenderer::applyWheelScaleAndPosition(float appliedScale) {
    // Compute vehicle size-based offsets for wheel placement
    std::array<float, 3> size = vehicle_.getSize();
    // Apply tuning factors to shrink track (width) and wheelbase (length)
    float halfWidth = size[0] * 0.5f * WHEEL_TRACK_FACTOR;
    float halfLength = size[2] * 0.5f * WHEELBASE_FACTOR;

    // Insets from edges so wheels sit under the fenders
    float xInset = 0.8f * appliedScale;   // move inward from the side
    float zFrontInset = 0.9f * appliedScale;
    float zRearInset = 0.9f * appliedScale;

    // Wheel Y-position (approximate wheel center height above ground)
    // Raise wheels by WHEEL_LIFT after applying car-lower adjustment
    float wheelCenterY = (0.35f * size[1] * appliedScale) - CAR_LOWER_AMOUNT + WHEEL_LIFT;

    // Applied wheel scale (in case models need scaling)
    float wheelScale = appliedScale;

    // Position front-left pivot and child wheel
    if (wheelFLPivot_) {
        // Add wheel geometry center offset (scaled) so the visual wheel keeps its intended position
        float pxFL = (-halfWidth + xInset) + (wheelCenterFL_[0] * wheelScale);
        float pyFL = wheelCenterY + (wheelCenterFL_[1] * wheelScale);
        float pzFL = (halfLength - zFrontInset) + (wheelCenterFL_[2] * wheelScale);
        wheelFLPivot_->position.set(pxFL, pyFL, pzFL);
        // Ensure pivot has no rotation initially
        wheelFLPivot_->rotation.set(0.0f, 0.0f, 0.0f);
        if (wheelFL_) {
            wheelFL_->scale.setScalar(wheelScale);
            // Keep wheel geometry centered on pivot so it spins around local X
            wheelFL_->position.set(0.0f, 0.0f, 0.0f);
            // Enable shadows on wheel meshes if present
            wheelFL_->traverse([](Object3D& obj) {
                if (auto mesh = obj.as<Mesh>()) {
                    mesh->castShadow = true;
                    mesh->receiveShadow = false;
                }
            });
        }
    }

    // Front-right
    if (wheelFRPivot_) {
        float pxFR = (halfWidth - xInset) + (wheelCenterFR_[0] * wheelScale);
        float pyFR = wheelCenterY + (wheelCenterFR_[1] * wheelScale);
        float pzFR = (halfLength - zFrontInset) + (wheelCenterFR_[2] * wheelScale);
        wheelFRPivot_->position.set(pxFR, pyFR, pzFR);
        wheelFRPivot_->rotation.set(0.0f, 0.0f, 0.0f);
        if (wheelFR_) {
            wheelFR_->scale.setScalar(wheelScale);
            wheelFR_->position.set(0.0f, 0.0f, 0.0f);
            wheelFR_->traverse([](Object3D& obj) {
                if (auto mesh = obj.as<Mesh>()) {
                    mesh->castShadow = true;
                    mesh->receiveShadow = false;
                }
            });
        }
    }

    // Rear-left
    if (wheelRLPivot_) {
        float pxRL = (-halfWidth + xInset) + (wheelCenterRL_[0] * wheelScale);
        float pyRL = wheelCenterY + (wheelCenterRL_[1] * wheelScale);
        float pzRL = (-halfLength + zRearInset) + (wheelCenterRL_[2] * wheelScale);
        wheelRLPivot_->position.set(pxRL, pyRL, pzRL);
        wheelRLPivot_->rotation.set(0.0f, 0.0f, 0.0f);
        if (wheelRL_) {
            wheelRL_->scale.setScalar(wheelScale);
            wheelRL_->position.set(0.0f, 0.0f, 0.0f);
            wheelRL_->traverse([](Object3D& obj) {
                if (auto mesh = obj.as<Mesh>()) {
                    mesh->castShadow = true;
                    mesh->receiveShadow = false;
                }
            });
        }
    }

    // Rear-right
    if (wheelRRPivot_) {
        float pxRR = (halfWidth - xInset) + (wheelCenterRR_[0] * wheelScale);
        float pyRR = wheelCenterY + (wheelCenterRR_[1] * wheelScale);
        float pzRR = (-halfLength + zRearInset) + (wheelCenterRR_[2] * wheelScale);
        wheelRRPivot_->position.set(pxRR, pyRR, pzRR);
        wheelRRPivot_->rotation.set(0.0f, 0.0f, 0.0f);
        if (wheelRR_) {
            wheelRR_->scale.setScalar(wheelScale);
            wheelRR_->position.set(0.0f, 0.0f, 0.0f);
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
// but here we also animate wheel spin (rotation around local X) and front wheel yaw.
void VehicleRenderer::update() {
    // Sync base position/rotation/visibility
    GameObjectRenderer::update();

    // Current vehicle transform
    auto pos = vehicle_.getPosition();
    float rot = vehicle_.getRotation();

    // Distance travelled in XZ plane since last update
    float dx = pos[0] - prevPosition_[0];
    float dz = pos[2] - prevPosition_[2];
    float distance = std::sqrt(dx * dx + dz * dz);

    // Estimate applied scale and wheel radius
    float appliedScale = MODEL_SCALE * modelScale_;
    std::array<float, 3> size = vehicle_.getSize();
    float wheelRadius = std::max(0.001f, size[1] * appliedScale * WHEEL_RADIUS_FACTOR);

    // Spin angle = distance / radius (radians)
    float spinDelta = distance / wheelRadius;

    // Direction of spin depends on forward/backward velocity
    float velocity = vehicle_.getVelocity();
    float spinDir = (velocity >= 0.0f) ? 1.0f : -1.0f;

    // Accumulate spin for each wheel (applied to the child wheel mesh)
    if (wheelFL_) wheelSpinFL_ += spinDir * spinDelta * 0.1;
    if (wheelFR_) wheelSpinFR_ += spinDir * spinDelta * 0.1;
    if (wheelRL_) wheelSpinRL_ += spinDir * spinDelta * 0.1;
    if (wheelRR_) wheelSpinRR_ += spinDir * spinDelta * 0.1;

    // Compute a steering yaw for front wheel pivots based on change in vehicle rotation
    float rotDelta = rot - prevRotation_;
    // Normalize to -PI..PI
    while (rotDelta > 3.14159265f) rotDelta -= 2.0f * 3.14159265f;
    while (rotDelta < -3.14159265f) rotDelta += 2.0f * 3.14159265f;

    float steerYaw = std::clamp(rotDelta * STEERING_VISUAL_MULTIPLIER, -MAX_WHEEL_STEER_ANGLE, MAX_WHEEL_STEER_ANGLE);

    // Apply rotations: pivot->rotation.y for steering, wheel mesh rotation.x for spin
    if (wheelFLPivot_) {
        wheelFLPivot_->rotation.y = steerYaw;
        if (wheelFL_) {
            switch (wheelSpinAxisFL_) {
                case WheelSpinAxis::X: wheelFL_->rotation.x = wheelInvertFL_ * wheelSpinFL_; break;
                case WheelSpinAxis::Y: wheelFL_->rotation.y = wheelInvertFL_ * wheelSpinFL_; break;
                case WheelSpinAxis::Z: wheelFL_->rotation.z = wheelInvertFL_ * wheelSpinFL_; break;
            }
        }
    }
    if (wheelFRPivot_) {
        wheelFRPivot_->rotation.y = steerYaw;
        if (wheelFR_) {
            switch (wheelSpinAxisFR_) {
                case WheelSpinAxis::X: wheelFR_->rotation.x = wheelInvertFR_ * wheelSpinFR_; break;
                case WheelSpinAxis::Y: wheelFR_->rotation.y = wheelInvertFR_ * wheelSpinFR_; break;
                case WheelSpinAxis::Z: wheelFR_->rotation.z = wheelInvertFR_ * wheelSpinFR_; break;
            }
        }
    }
    if (wheelRLPivot_) {
        wheelRLPivot_->rotation.y = 0.0f; // rear wheels don't steer
        if (wheelRL_) {
            switch (wheelSpinAxisRL_) {
                case WheelSpinAxis::X: wheelRL_->rotation.x = wheelInvertRL_ * wheelSpinRL_; break;
                case WheelSpinAxis::Y: wheelRL_->rotation.y = wheelInvertRL_ * wheelSpinRL_; break;
                case WheelSpinAxis::Z: wheelRL_->rotation.z = wheelInvertRL_ * wheelSpinRL_; break;
            }
        }
    }
    if (wheelRRPivot_) {
        wheelRRPivot_->rotation.y = 0.0f;
        if (wheelRR_) {
            switch (wheelSpinAxisRR_) {
                case WheelSpinAxis::X: wheelRR_->rotation.x = wheelInvertRR_ * wheelSpinRR_; break;
                case WheelSpinAxis::Y: wheelRR_->rotation.y = wheelInvertRR_ * wheelSpinRR_; break;
                case WheelSpinAxis::Z: wheelRR_->rotation.z = wheelInvertRR_ * wheelSpinRR_; break;
            }
        }
    }

    // Save for next frame
    prevPosition_ = pos;
    prevRotation_ = rot;
}
