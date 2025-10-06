# Bilsimulator

A 3D vehicle simulator built with modern C++ and threepp, featuring realistic physics, dynamic audio, and a clean architecture designed for extensibility.

## Overview

This project is a 3D driving simulator where players can control a vehicle in a 3D environment. It demonstrates modern C++20 principles, object-oriented design, separation of concerns, and comprehensive unit testing.

## Features

### Core Functionality
- **Realistic vehicle physics** - Acceleration, friction, momentum, and speed-sensitive steering
- **AABB collision detection** - Axis-Aligned Bounding Box system for object interactions
- **Smooth camera follow** - Interpolated camera tracking with adjustable distance
- **Dynamic controls** - Responsive input handling with keyboard support

### Visual & Audio
- **7-segment speedometer** - Custom-built digital display (0-150 km/h)
- **Minimap** - Top-down view for spatial awareness
- **Dynamic engine audio** - Pitch and volume adjust based on vehicle speed
- **Shadow rendering** - Real-time shadows for depth perception

### Technical Highlights
- **Modular architecture** - Clear separation between logic, rendering, input, audio, and UI
- **Modern C++20** - Smart pointers, std::array, anonymous namespaces, const correctness
- **Comprehensive testing** - 20 unit tests with Catch2 (100% passing)
- **CMake build system** - Clean modular structure with automatic dependency management

## Quick Start

### Build
```bash
cmake -B build
cmake --build build
```

### Run
```bash
./build/src/bilsim
```

### Test
```bash
./run_tests.sh
# or
cd build && ctest --output-on-failure
```

## Controls

| Key | Action |
|-----|--------|
| **W** or **↑** | Accelerate forward |
| **S** or **↓** | Brake / Reverse |
| **A** or **←** | Turn left |
| **D** or **→** | Turn right |
| **R** | Reset vehicle |

## Architecture

### Design Philosophy

The project follows key software engineering principles:
- **Separation of Concerns** - Logic, rendering, input, audio, and UI are isolated modules
- **Low Coupling, High Cohesion** - Each module has a single, well-defined responsibility
- **Open/Closed Principle** - Easy to extend with new object types without modifying existing code
- **No Global Variables** - Anonymous namespaces for file-local constants

### Class Structure

```
GameObject (abstract)           GameObjectRenderer (abstract)
├── position, rotation, size    ├── scene, mesh, materials
├── intersects()                ├── update()
└── update() [pure virtual]     └── createModel() [virtual]
    │                               │
    └── Vehicle                     └── VehicleRenderer
        ├── velocity                    └── Custom car model
        ├── accelerate()
        └── turn()

Support Classes:
- SceneManager:  3D scene, camera, lighting, ground
- InputHandler:  Keyboard input processing
- AudioManager:  Sound playback with dynamic pitch
- UIManager:     HUD rendering (speedometer, minimap border)
```

### Project Structure

```
src/
├── core/               # Game logic (no graphics dependencies)
│   ├── gameObject.*    # Base class for all entities
│   └── vehicle.*       # Vehicle physics and state
├── graphics/           # Rendering (no game logic)
│   ├── gameObjectRenderer.*   # Generic renderer base
│   ├── vehicleRenderer.*      # Vehicle-specific rendering
│   └── sceneManager.*         # Scene setup and camera
├── input/
│   └── inputHandler.*  # Keyboard input handling
├── audio/
│   └── audioManager.*  # Sound playback
└── ui/
    └── uiManager.*     # HUD and UI rendering

tests/
├── test_vehicle.cpp       # Physics and movement tests
├── test_gameObject.cpp    # Base class tests
└── test_collision.cpp     # AABB collision tests
```

## Testing

The project includes comprehensive unit tests using Catch2:

**Test Coverage:**
- ✅ 7 Vehicle tests (acceleration, max speed, friction, turning, reset)
- ✅ 4 GameObject tests (initialization, setters, reset)
- ✅ 9 Collision tests (overlap detection, inactive objects, symmetry)

**Result:** 20/20 tests passing

```bash
# Run all tests
./run_tests.sh

# Run specific test
cd build && ./tests/run_tests "Vehicle acceleration"

# Verbose output
cd build && ctest --output-on-failure
```

## Implementation Details

### Physics Model

The vehicle physics simulate realistic driving behavior:
- **Acceleration**: 15 m/s² forward, 20 m/s² reverse
- **Max Speed**: 41.67 m/s (~150 km/h)
- **Friction**: 5 m/s² deceleration when not accelerating
- **Turn Rate**: 90°/s at speed, scales quadratically down to 0.2x at very low speeds (<0.3 m/s)

This creates natural-feeling controls where the car is responsive at speed but doesn't "spin in place" when nearly stationary.

### Rendering Pipeline

1. **Main View**: Full-screen 3D scene with perspective camera
2. **Minimap**: Top-down orthographic view in corner viewport
3. **UI Overlay**: 2D HUD elements rendered last

The renderer uses viewport/scissor test for multiple views in a single frame.

### Collision Detection

Simple AABB (Axis-Aligned Bounding Box) implementation:
- Fast and efficient for rectangular objects
- Works for all GameObject-derived types
- Easily extensible for pickup items, obstacles, etc.

## Dependencies

All dependencies are automatically fetched by CMake:

- **[threepp](https://github.com/markaren/threepp)** - 3D rendering (OpenGL wrapper inspired by three.js)
- **[miniaudio](https://github.com/mackron/miniaudio)** - Cross-platform audio playback
- **[Catch2](https://github.com/catchorg/Catch2)** v3.4.0 - Modern C++ testing framework

**Requirements:**
- CMake ≥ 3.21
- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- OpenGL 3.3+

## Future Enhancements

### Planned Features
- **Collectible objects** - Pickups that modify vehicle properties (speed boost, size change)
- **Obstacles** - Static and dynamic objects to avoid
- **Multiple vehicles** - Different vehicle types with unique characteristics
- **Advanced physics** - Integration with Bullet or PhysX for more realistic behavior
- **3D models** - Import .obj/.gltf models instead of geometric primitives
- **Particle effects** - Dust trails, smoke, visual feedback

### Code Improvements
- **Game class** - Move main loop logic out of main.cpp
- **CollisionManager** - Centralize collision detection and response
- **Config files** - External configuration for physics constants
- **Object pooling** - Better performance with many objects
- **Spatial partitioning** - Quadtree for efficient collision detection at scale

## Development Notes

### Code Style
- **camelCase** for variables and functions
- **PascalCase** for classes
- **Trailing underscore** for private member variables
- **Const correctness** enforced throughout
- **Smart pointers** used for all dynamic allocations

### Building for Development
```bash
# Debug build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Release build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Clean rebuild
rm -rf build && cmake -B build && cmake --build build
```

### Adding New Object Types

1. Create class inheriting from `GameObject`
2. Implement `update()` method
3. Create renderer inheriting from `GameObjectRenderer`
4. Implement `createModel()` method
5. Instantiate in main.cpp

Collision detection and rendering pipeline work automatically!

## License

This project was created as a learning exercise. Feel free to use and modify for educational purposes.

## Credits

- **threepp** by [@markaren](https://github.com/markaren) - 3D rendering library
- **Catch2** by [@catchorg](https://github.com/catchorg) - Testing framework
- **miniaudio** by [@mackron](https://github.com/mackron) - Audio library
