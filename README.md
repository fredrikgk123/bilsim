# Bilsimulator

**Kandidatnummer:** [LEGG INN DITT KANDIDATNUMMER HER]

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

### UML Class Diagram

```
┌─────────────────────────┐
│      GameObject         │ (abstract)
├─────────────────────────┤
│ - position_: array<3>   │
│ - rotation_: float      │
│ - size_: array<3>       │
│ - active_: bool         │
├─────────────────────────┤
│ + update(dt): void      │ (pure virtual)
│ + reset(): void         │
│ + intersects(obj): bool │
│ + getPosition(): array  │
│ + setActive(b): void    │
└───────────┬─────────────┘
            │
      ┌─────┴──────┐
      │            │
┌─────▼──────┐ ┌──▼────────┐
│  Vehicle   │ │  Powerup  │
├────────────┤ ├───────────┤
│ - velocity_│ │ - type_   │
│ - accel_   │ │           │
├────────────┤ ├───────────┤
│ + accel()  │ │ + update()│
│ + turn()   │ │           │
└────────────┘ └───────────┘


┌─────────────────��────────┐
│  GameObjectRenderer      │ (abstract)
├──────────────────────────┤
│ - scene_: Scene&         │
│ - gameObject_: GameObject&│
│ - objectGroup_: Group*   │
├──────────────────────────┤
│ + update(): void         │
│ # createModel(): void    │ (virtual)
└────────────┬─────────────┘
             │
      ┌──────┴────────┐
      │               │
┌─────▼────────┐ ┌───▼────────────┐
│VehicleRenderer│ │PowerupRenderer │
├──────────────┤ ├────────────────┤
│# createModel()│ │# createModel() │
└──────────────┘ └────────────────┘


┌────────────────┐     ┌─────────────┐
│ SceneManager   │     │InputHandler │
├────────────────┤     ├─────────────┤
│ - scene_       │     │ - vehicle_& │
│ - camera_      │     │ - keyStates │
│ - renderer_    │     ├─────────────┤
├────────────────┤     │ + onKey()   │
│ + setupScene() │     │ + update()  │
│ + render()     │     └─────────────┘
└────────────────┘

┌─────────────┐     ┌────────────┐
│AudioManager │     │ UIManager  │
├─────────────┤     ├─────────────┤
│ - engine_*  │     │ - renderer_│
│ - sound_*   │     │ - hudScene_│
├─────────────┤     ├─────────────┤
│ + update()  │     │ + render() │
└─────────────┘     └─────────────┘
```

**Relationships:**
- Vehicle/Powerup **inherit from** GameObject
- VehicleRenderer/PowerupRenderer **inherit from** GameObjectRenderer
- InputHandler **holds reference to** Vehicle
- All managers are **independent** with minimal coupling
- Core module has **no dependencies** on graphics/ui/audio

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

---

## Refleksjon (Reflection)

### Hva jeg er fornøyd med (What I'm satisfied with)

#### 1. Arkitektur og designprinsipper
Jeg er særlig stolt av hvordan prosjektet følger god programvarearkitektur. Den strenge separasjonen mellom `core/` (logikk) og `graphics/` (visualisering) gjør at:
- Core-modulen kan testes helt uten å initialisere OpenGL
- Nye renderere kan lages uten å endre spillogikken
- Koden følger Dependency Inversion Principle - core avhenger ikke av grafikk

Dette demonstreres tydelig i at alle unit tests kjører på core-modulen uten noen grafikk-avhengigheter.

#### 2. Moderne C++20 praksis
Konsekvent bruk av moderne C++ features:
```cpp
// Smart pointers - automatic memory management
std::unique_ptr<Vehicle> vehicle_;           // Eierskap
std::shared_ptr<Scene> scene_;              // Delt eierskap
const Vehicle& vehicle_;                     // Ikke-eiende referanse

// RAII - ingen manuell cleanup
~AudioManager() = default;  // unique_ptr håndterer alt

// Anonymous namespaces - ikke globale variabler
namespace {
    const float MAX_SPEED = 41.67f;  // File-local
}
```

Dette eliminerer memory leaks og gjør koden tryggere og mer vedlikeholdbar.

#### 3. Testbarhet
20 unit tests med 100% pass rate. Alle kritiske komponenter (Vehicle, GameObject, collision detection) er grundig testet. Catch2 framework gjør testene lesbare og lette å utvide.

#### 4. Fysikk-simulering
Realistisk kjørefølelse med:
- Hastighetsavhengig svingradius (bil spinner ikke på stedet ved lav hastighet)
- Eksponensiell friksjon
- Momentum-bevaring
- Smooth camera interpolation (lerp)

#### 5. UI implementasjon
Custom 7-segment display for speedometer uten eksterne UI-biblioteker. Dette viser forståelse for hvordan 3D-grafikk kan brukes kreativt for UI-elementer.

### Hva som kunne vært bedre (What could be improved)

#### 1. Manglende obstacles/hindringer
**Problem:** Kravspesifikasjonen nevner "Hindringer, fare-elementer, dører, portaler o.l." under Miljø, men dette er ikke implementert.

**Hvordan løse:**
```cpp
// 1. Lag src/core/obstacle.hpp
class Obstacle : public GameObject {
public:
    Obstacle(float x, float y, float z, float w, float h, float l);
    void update(float deltaTime) override;  // Statisk, gjør ingenting
};

// 2. Lag src/graphics/obstacleRenderer.hpp
class ObstacleRenderer : public GameObjectRenderer {
protected:
    void createModel() override;  // Tegn vegg/hindring
};

// 3. I main.cpp: Håndter kollisjon
if (vehicle.intersects(obstacle)) {
    // Push vehicle back basert på collision normal
    // Eller implementer en CollisionManager
}
```

**Teknisk innsikt:** GameObject-hierarkiet er allerede designet for dette (Open/Closed Principle), så det ville vært rett frem å legge til.

#### 2. Begrenset feilhåndtering
**Problem:** Koden har minimal error handling:
```cpp
bool audioEnabled = audioManager.initialize("carnoise.wav");
// Fortsetter uten audio hvis fil mangler
```

**Forbedring:**
- Parameter-validering i funksjoner (sjekk for nullptr, ugyldige verdier)
- Exception handling for kritiske feil
- Boundary checking på vektorer før tilgang
- Assert statements for debug builds

**Hvordan:**
```cpp
void Vehicle::setPosition(float x, float y, float z) {
    // Validate input
    if (std::isnan(x) || std::isnan(y) || std::isnan(z)) {
        throw std::invalid_argument("Position contains NaN");
    }
    position_ = {x, y, z};
}
```

#### 3. Test coverage kun på core-modulen
**Problem:** Graphics, audio, input og ui-modulene har ingen unit tests.

**Hvorfor:** Disse modulene er vanskeligere å teste fordi de avhenger av eksterne systemer (OpenGL, audio devices, keyboard).

**Løsning:**
- Integration tests som starter hele applikasjonen
- Mock objects for å teste input/audio uten faktisk hardware
- Renderer tests med headless OpenGL context

#### 4. Manglende CI/CD pipeline
**Problem:** Ingen GitHub Actions workflow for automatisk bygging og testing.

**Hvordan implementere:**
```yaml
# .github/workflows/build-and-test.yml
name: Build and Test
on: [push, pull_request]
jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    steps:
      - uses: actions/checkout@v3
      - name: Build
        run: |
          cmake -B build
          cmake --build build
      - name: Test
        run: cd build && ctest --output-on-failure
```

**Fordel:** Verifiserer cross-platform kompatibilitet automatisk.

#### 5. Main.cpp er for stor
**Problem:** main.cpp inneholder mye game loop logikk (~150 linjer).

**Bedre design:**
```cpp
// src/core/game.hpp
class Game {
public:
    void initialize();
    void run();
    void update(float deltaTime);
    void render();
private:
    std::unique_ptr<Vehicle> vehicle_;
    std::vector<std::unique_ptr<Powerup>> powerups_;
    std::unique_ptr<SceneManager> sceneManager_;
    // ... etc
};

// main.cpp blir da bare:
int main() {
    Game game;
    game.run();
    return 0;
}
```

**Fordel:** Lettere å teste, bedre Single Responsibility Principle.

### Teknisk innsikt i valgte løsninger

#### Hvorfor GameObject som abstrakt baseklasse?
Dette følger **Template Method pattern** og gjør at collision detection fungerer polymorfisk:
```cpp
bool GameObject::intersects(const GameObject& other) const {
    // Fungerer for Vehicle, Powerup, eller fremtidige typer!
}
```

#### Hvorfor GameObjectRenderer hierarchy?
**Separation of Concerns** - rendering skal ikke blandes med logikk:
- `Vehicle` vet ingenting om threepp eller OpenGL
- `VehicleRenderer` vet ingenting om fysikk eller hastighet
- De kommuniserer kun gjennom GameObject interface

Dette gjør at jeg kan:
- Teste Vehicle uten grafikk
- Bytte renderer uten å endre Vehicle
- Legge til nye render-stiler (wireframe, textured, etc.) uten å endre core

#### Hvorfor anonymous namespaces for konstanter?
```cpp
namespace {
    const float MAX_SPEED = 41.67f;
}
```

**I stedet for globale variabler eller #define:**
- Type-safe (ikke makro)
- Begrenset scope (kun synlig i denne filen)
- Unngår linking conflicts hvis flere filer har samme navn
- Moderne C++ best practice

### Konklusjon

Prosjektet demonstrerer solid forståelse for objektorientert programmering, moderne C++, og god programvarearkitektur. De største styrkene er:
- Ren separasjon mellom logikk og presentasjon
- Testbar kodebase
- Utvidbar design (Open/Closed Principle)

Hovedforbedringsområdet er å implementere obstacles for å møte alle miljø-krav, samt legge til CI/CD for automatisk testing.

Totalt sett er jeg fornøyd med den tekniske kvaliteten, selv om funksjonaliteten kunne vært mer komplett.
