# Bilsimulator - Architecture & Naming Conventions

**Project:** Bilsimulator (Car Racing Game)  
**Date:** November 11, 2025  
**Version:** 1.0

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Architecture](#architecture)
3. [Naming Conventions](#naming-conventions)
4. [Code Style Guidelines](#code-style-guidelines)
5. [Directory Structure](#directory-structure)
6. [Design Patterns](#design-patterns)
7. [Dependencies](#dependencies)

---

## Project Overview

Bilsimulator is a 3D car racing game built with C++20, featuring realistic physics, multiple camera modes, audio feedback, and a comprehensive UI system.

### Key Features
- Real-time 3D rendering with threepp
- Physics-based vehicle simulation
- Multiple camera modes (Follow, Interior)
- RPM-based engine sounds
- Drift mechanics and nitrous boost
- Dynamic obstacle system
- Minimap with window-responsive scaling
- ImGui-based dashboard UI

---

## Architecture

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Main Application                      │
│                      (main.cpp)                          │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│                   Game (core/game)                       │
│          Central coordinator and game loop               │
└──┬────────┬────────┬────────┬────────┬─────────┬────────┘
   │        │        │        │        │         │
   ▼        ▼        ▼        ▼        ▼         ▼
┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐
│Scene │ │Input │ │Audio │ │  UI  │ │Core  │ │Mgrs  │
│Mgr   │ │      │ │      │ │      │ │Logic │ │      │
└──────┘ └──────┘ └──────┘ └──────┘ └──────┘ └──────┘
```

### Layer Separation

#### 1. **Core Layer** (`src/core/`)
**Purpose:** Game logic, physics, and data models

**Components:**
- `Vehicle` - Vehicle physics and state management
- `GameObject` - Base class for all game objects
- `Obstacle` - Static obstacles (trees, walls)
- `Powerup` - Collectible powerups
- `ObstacleManager` - Manages obstacle generation and collisions
- `PowerupManager` - Manages powerup spawning and collection
- `Game` - Central game coordinator

**Responsibilities:**
- Physics calculations
- Collision detection
- State management
- Game loop coordination

**Dependencies:** None (core is independent)

#### 2. **Graphics Layer** (`src/graphics/`)
**Purpose:** Visual representation and rendering

**Components:**
- `SceneManager` - 3D scene, camera, and lighting management
- `GameObjectRenderer` - Base renderer for game objects
- `VehicleRenderer` - Renders vehicle with model loading
- `ObstacleRenderer` - Renders obstacles (trees, walls)
- `PowerupRenderer` - Renders powerups with animations

**Responsibilities:**
- 3D rendering with threepp
- Camera management (multiple modes)
- Model loading (OBJ files)
- Visual effects and animations

**Dependencies:** Core layer (reads state via interfaces)

#### 3. **Input Layer** (`src/input/`)
**Purpose:** User input handling

**Components:**
- `InputHandler` - Keyboard input and vehicle control

**Responsibilities:**
- Keyboard event processing
- Translating input to game actions
- Camera mode switching

**Dependencies:** Core layer (controls via `IControllable`)

#### 4. **Audio Layer** (`src/audio/`)
**Purpose:** Sound effects and audio management

**Components:**
- `AudioManager` - Audio playback and control

**Responsibilities:**
- Engine sound synthesis (RPM-based)
- Drift sound effects
- Audio state management

**Dependencies:** Core layer (reads state via `IVehicleState`)

#### 5. **UI Layer** (`src/ui/`)
**Purpose:** User interface and HUD

**Components:**
- `ImGuiContext` - ImGui initialization and lifecycle
- `ImGuiLayer` - Dashboard, speedometer, gear indicator

**Responsibilities:**
- Dashboard rendering
- Speedometer and RPM display
- Gear indicator
- UI scaling

**Dependencies:** Core layer (reads vehicle state)

### Data Flow

```
┌──────────────┐
│   User Input │
└──────┬───────┘
       │
       ▼
┌──────────────┐     ┌─────────────┐
│ InputHandler │────▶│  Vehicle    │
└──────────────┘     │  (Physics)  │
                     └──────┬──────┘
                            │
                     ┌──────▼──────────────────┐
                     │  Update Game State      │
                     │  - Physics              │
                     │  - Collisions           │
                     │  - Managers             │
                     └──────┬──────────────────┘
                            │
       ┌────────────────────┼────────────────────┐
       │                    │                    │
       ▼                    ▼                    ▼
┌─────────────┐      ┌─────────────┐     ┌─────────────┐
│  Renderers  │      │AudioManager │     │ ImGuiLayer  │
│  (Graphics) │      │  (Audio)    │     │    (UI)     │
└─────────────┘      └─────────────┘     └─────────────┘
       │                    │                    │
       └────────────────────┼────────────────────┘
                            │
                            ▼
                     ┌─────────────┐
                     │   Display   │
                     └─────────────┘
```

### Interface-Based Design

#### `IVehicleState` (Read-Only)
**Purpose:** Safe read-only access to vehicle state

```cpp
class IVehicleState {
    virtual float getVelocity() const = 0;
    virtual float getRPM() const = 0;
    virtual int getCurrentGear() const = 0;
    virtual bool isNitrousActive() const = 0;
    virtual bool isDrifting() const = 0;
    // ... more getters
};
```

**Used by:**
- AudioManager (RPM, drift state)
- VehicleRenderer (steering input)
- ImGuiLayer (speedometer, gear display)

#### `IControllable` (Write-Only)
**Purpose:** Control interface for input

```cpp
class IControllable {
    virtual void accelerateForward(float multiplier) = 0;
    virtual void accelerateBackward() = 0;
    virtual void turn(float amount) = 0;
    virtual void startDrift() = 0;
    virtual void stopDrift() = 0;
    // ... more control methods
};
```

**Used by:**
- InputHandler (keyboard → vehicle control)

**Benefits:**
- ✅ Clear separation between reading and writing
- ✅ Prevents unwanted state modifications
- ✅ Easy to test and mock
- ✅ Thread-safe design potential

---

## Naming Conventions

### Variables

#### Local Variables: `camelCase`
```cpp
float deltaTime = 0.016f;
int minimapSize = 150;
bool validPosition = true;
float distanceFromCenter = 10.0f;
```

#### Member Variables: `camelCase_` (trailing underscore)
```cpp
class Vehicle {
    float velocity_;
    float rotation_;
    bool nitrousActive_;
    std::unique_ptr<AudioManager> audioManager_;
};
```

#### Constants: `UPPER_SNAKE_CASE`
```cpp
constexpr float MAX_SPEED = 50.0f;
constexpr int MINIMAP_SIZE = 150;
constexpr float BASE_CAMERA_DISTANCE = 8.0f;
```

#### Global/Static Constants: `UPPER_SNAKE_CASE`
```cpp
namespace {
    constexpr float CAMERA_FOV_MIN = 75.0f;
    constexpr float DRIFT_ANGLE_THRESHOLD = 0.2f;
}
```

### Functions and Methods

#### Functions/Methods: `camelCase`
```cpp
void updateCamera();
float calculateTurnRate() const;
bool checkCircleCollision();
void renderMainView();
```

#### Getters: `getSomething()`
```cpp
float getVelocity() const;
int getCurrentGear() const;
bool isNitrousActive() const;  // Boolean: is/has prefix
```

#### Setters: `setSomething()`
```cpp
void setVelocity(float velocity);
void setPosition(float x, float y, float z);
void setActive(bool active);
```

### Classes and Types

#### Classes: `PascalCase`
```cpp
class Vehicle;
class SceneManager;
class VehicleRenderer;
class AudioManager;
```

#### Interfaces: `IPascalCase` (I prefix)
```cpp
class IVehicleState;
class IControllable;
```

#### Enums: `PascalCase` for type, `UPPER_CASE` for values
```cpp
enum class CameraMode {
    FOLLOW,
    INTERIOR,
    SIDE
};

enum class ObstacleType {
    TREE,
    WALL
};
```

#### Structs: `PascalCase`
```cpp
struct AudioDeleter;
struct CollisionResult;
```

### Namespaces

#### Namespaces: `PascalCase` or `snake_case`
```cpp
namespace GameConfig {
    namespace World { }
    namespace Powerup { }
    namespace Obstacle { }
}

namespace VehicleTuning {
    constexpr float MAX_SPEED = 50.0f;
}
```

#### Anonymous Namespaces: For file-local constants
```cpp
namespace {
    constexpr float CAMERA_NEAR = 0.1f;
    constexpr float CAMERA_FAR = 1000.0f;
}
```

### Files

#### Header Files: `snake_case.hpp`
```
vehicle.hpp
scene_manager.hpp
game_object_renderer.hpp
```

#### Source Files: `snake_case.cpp`
```
vehicle.cpp
scene_manager.cpp
game_object_renderer.cpp
```

#### Configuration Headers: `snake_case.hpp`
```
game_config.hpp
vehicle_tuning.hpp
object_sizes.hpp
```

---

## Code Style Guidelines

### General Principles

1. **Const Correctness**
   - Use `const` for values that don't change
   - Mark methods `const` if they don't modify state
   - Use `constexpr` for compile-time constants

2. **Early Returns**
   - Use guard clauses for validation
   - Return early to reduce nesting
   ```cpp
   void render() {
       if (!sceneManager_) return;
       // ... main logic
   }
   ```

3. **Smart Pointers**
   - Use `std::unique_ptr` for owned resources
   - Use `std::shared_ptr` for shared ownership
   - Avoid raw pointers for ownership

4. **No Magic Numbers**
   - Use named constants
   - Define in `GameConfig` or anonymous namespace
   ```cpp
   // BAD
   const int size = 150;
   
   // GOOD
   const int size = GameConfig::UI::MINIMAP_SIZE;
   ```

### Comments

#### Section Headers
```cpp
// ============================================
// Initialization Methods
// ============================================
```

#### Function Documentation
```cpp
// Calculate turn rate based on current velocity
// Returns 0.0-1.0 multiplier for turn responsiveness
float calculateTurnRate() const noexcept;
```

#### Inline Comments
```cpp
// Don't turn if completely stopped
if (absoluteVelocity < VehicleTuning::MIN_SPEED_THRESHOLD) {
    return 0.0f;
}
```

#### Comment Style
- Use `//` for single-line comments
- Space after `//`
- Keep comments concise and meaningful
- Explain "why", not "what"

### Formatting

#### Braces
```cpp
// Opening brace on same line
void function() {
    if (condition) {
        // code
    } else {
        // code
    }
}
```

#### Indentation
- 4 spaces (no tabs)
- Consistent alignment

#### Line Length
- Aim for < 120 characters
- Break long lines at logical points

#### Spacing
```cpp
// Space after keywords
if (condition) { }
for (int i = 0; i < n; ++i) { }

// Space around operators
int result = a + b * c;
bool flag = (x > 0) && (y < 10);

// No space after function names
void function(int arg);
```

---

## Directory Structure

```
bilsim/
├── src/
│   ├── main.cpp                    # Entry point
│   │
│   ├── core/                       # Core game logic
│   │   ├── game.hpp/.cpp          # Main game coordinator
│   │   ├── game_config.hpp        # Configuration constants
│   │   ├── vehicle.hpp/.cpp       # Vehicle physics
│   │   ├── vehicle_tuning.hpp     # Vehicle tuning constants
│   │   ├── game_object.hpp/.cpp   # Base game object
│   │   ├── obstacle.hpp/.cpp      # Obstacle entity
│   │   ├── obstacle_manager.hpp/.cpp  # Obstacle management
│   │   ├── powerup.hpp/.cpp       # Powerup entity
│   │   ├── powerup_manager.hpp/.cpp   # Powerup management
│   │   ├── logger.hpp             # Logging utilities
│   │   ├── object_sizes.hpp       # Object size constants
│   │   └── random_position_generator.hpp  # Position generation
│   │
│   ├── graphics/                   # Rendering layer
│   │   ├── scene_manager.hpp/.cpp # Scene and camera management
│   │   ├── game_object_renderer.hpp/.cpp  # Base renderer
│   │   ├── vehicle_renderer.hpp/.cpp      # Vehicle rendering
│   │   ├── obstacle_renderer.hpp/.cpp     # Obstacle rendering
│   │   └── powerup_renderer.hpp/.cpp      # Powerup rendering
│   │
│   ├── input/                      # Input handling
│   │   └── input_handler.hpp/.cpp # Keyboard input
│   │
│   ├── audio/                      # Audio system
│   │   └── audio_manager.hpp/.cpp # Sound management
│   │
│   └── ui/                         # User interface
│       ├── imgui_context.hpp/.cpp # ImGui initialization
│       └── imgui_layer.hpp/.cpp   # Dashboard UI
│
├── tests/                          # Unit tests
│   ├── test_vehicle.cpp
│   ├── test_gameObject.cpp
│   ├── test_collision.cpp
│   └── test_validation.cpp
│
├── assets/                         # Game assets
│   ├── body.obj/.mtl              # Car body model
│   ├── steeringwheel.obj/.mtl     # Steering wheel model
│   ├── wheels/                    # Wheel models (FL/FR/RL/RR)
│   ├── carnoise.wav               # Engine sound
│   └── tireScreech.wav            # Drift sound
│
├── cmake-build-debug-cmake/        # Build output (generated)
├── CMakeLists.txt                  # CMake configuration
└── README.md                       # Project documentation
```

### File Organization

#### Header Files (.hpp)
```cpp
#pragma once

// System includes
#include <memory>
#include <vector>

// Library includes
#include <threepp/threepp.hpp>

// Project includes
#include "game_object.hpp"

// Forward declarations
class Vehicle;

// Class declaration
class MyClass {
    // ...
};
```

#### Source Files (.cpp)
```cpp
#include "my_class.hpp"

// Additional includes
#include <cmath>

// Anonymous namespace for file-local constants
namespace {
    constexpr float SOME_CONSTANT = 1.0f;
}

// Implementation
MyClass::MyClass() {
    // ...
}
```

---

## Design Patterns

### 1. **Separation of Concerns**
- Core logic independent of rendering
- Renderers read state via interfaces
- Clear boundaries between layers

### 2. **Interface Segregation**
- `IVehicleState` for read-only access
- `IControllable` for write-only control
- Small, focused interfaces

### 3. **Dependency Injection**
```cpp
Game::Game(threepp::Canvas& canvas)
    : canvas_(canvas) { }
```

### 4. **Manager Pattern**
- `ObstacleManager` for obstacles
- `PowerupManager` for powerups
- Encapsulates collection management

### 5. **Renderer Pattern**
```cpp
class GameObjectRenderer {
    GameObject& gameObject_;  // Reference to game object
    
    void update() {
        // Sync visual with game object
    }
};
```

### 6. **RAII (Resource Acquisition Is Initialization)**
```cpp
std::unique_ptr<AudioManager> audioManager_;
std::unique_ptr<SceneManager> sceneManager_;
// Automatic cleanup via smart pointers
```

### 7. **Configuration Objects**
```cpp
namespace GameConfig {
    namespace World {
        inline constexpr float PLAY_AREA_SIZE = 200.0f;
    }
}
```

---

## Dependencies

### External Libraries

#### Core Dependencies
- **threepp** (v0.x) - 3D rendering engine
- **GLFW** - Window and input management
- **OpenGL** - Graphics API
- **GLM** - Mathematics library (via threepp)

#### Audio
- **miniaudio** - Single-header audio library

#### UI
- **ImGui** (v1.92.4) - Immediate mode GUI

#### Testing
- **Catch2** (v3.4.0) - Unit testing framework

### Build System
- **CMake** (≥3.21) - Build configuration
- **Ninja** - Build system (optional)

### Dependency Management
```cmake
# CMakeLists.txt
FetchContent_Declare(threepp ...)
FetchContent_Declare(Catch2 ...)
FetchContent_Declare(imgui ...)
FetchContent_MakeAvailable(threepp Catch2 imgui)
```

---

## Best Practices

### Memory Management
✅ **DO:**
- Use smart pointers for ownership
- Use references for non-owning access
- RAII for resource management

❌ **DON'T:**
- Use raw pointers for ownership
- Manual `new`/`delete`
- Memory leaks

### Error Handling
✅ **DO:**
- Check for null pointers
- Validate parameters
- Use guard clauses
- Return early on error

❌ **DON'T:**
- Assume resources are valid
- Ignore error conditions
- Deep nesting

### Performance
✅ **DO:**
- Update only what changes
- Use `const` and `noexcept`
- Pass large objects by const reference
- Cache expensive calculations

❌ **DON'T:**
- Update static objects every frame
- Copy large objects unnecessarily
- Recalculate constants

### Testing
✅ **DO:**
- Test public interfaces
- Test edge cases
- Keep tests simple and focused

❌ **DON'T:**
- Test private methods directly
- Create brittle tests
- Ignore failing tests

---

## Configuration Constants

### Organized in Namespaces

```cpp
namespace GameConfig {
    namespace World {
        inline constexpr float PLAY_AREA_SIZE = 200.0f;
        inline constexpr float SPAWN_POINT_X = 0.0f;
        inline constexpr float SPAWN_POINT_Y = 0.0f;
        inline constexpr float SPAWN_POINT_Z = 0.0f;
    }
    
    namespace UI {
        inline constexpr int MINIMAP_SIZE = 150;
        inline constexpr int MINIMAP_PADDING = 10;
    }
    
    namespace Assets {
        inline constexpr const char* CAR_MODEL_PATH = "assets/body.obj";
        inline constexpr const char* ENGINE_SOUND_PATH = "assets/carnoise.wav";
    }
}
```

### Benefits
- Centralized configuration
- Easy to find and modify
- Type-safe
- No magic numbers in code

---

## Summary

### Key Principles

1. **Separation of Concerns** - Each layer has a clear responsibility
2. **Interface-Based Design** - Read/write separation via interfaces
3. **Consistent Naming** - camelCase locals, camelCase_ members, UPPER_SNAKE_CASE constants
4. **Smart Pointers** - RAII for automatic resource management
5. **Configuration First** - Named constants instead of magic numbers
6. **Clean Code** - Early returns, const correctness, meaningful names

### Code Quality Checklist

- [ ] Variables use correct naming convention
- [ ] No magic numbers (use GameConfig)
- [ ] Const correctness applied
- [ ] Smart pointers for ownership
- [ ] Interfaces for cross-layer communication
- [ ] Early returns for validation
- [ ] Comments explain "why", not "what"
- [ ] No memory leaks
- [ ] Tests pass
- [ ] Code compiles with no warnings

---

**Document Version:** 1.0  
**Last Updated:** November 11, 2025  
**Maintainer:** Bilsimulator Development Team


