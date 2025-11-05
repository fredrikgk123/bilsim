# Bilsimulator

**Kandidatnummer:** [LEGG INN DITT KANDIDATNUMMER HER]

## Introduction

Bilsimulator is a 3D vehicle simulator built with modern C++20 and advanced graphics rendering. The application demonstrates arcade-style vehicle physics, dynamic audio feedback, and a modular software architecture designed for extensibility and maintainability. 

Players control a vehicle in a 3D environment with speed-sensitive steering, drift mechanics, and nitrous boost pickups. The simulation features real-time collision detection, dynamic camera systems, and responsive audio that adjusts to vehicle speed and state. The project emphasizes clean code architecture with separation of concerns, comprehensive unit testing, and adherence to modern C++ best practices.

### Key Features
- Arcade-style vehicle physics with velocity-based movement
- Speed-sensitive steering for authentic driving feel
- Drift mechanics with angle accumulation and decay
- Nitrous boost powerup system
- Dynamic camera with follow and hood-cam modes
- 2D AABB collision detection for powerup collection
- Speed-reactive audio engine with pitch and volume modulation
- Custom 7-segment digital speedometer (0-150 km/h)
- Top-down minimap for spatial awareness
- Real-time shadow rendering

### Controls
- **W/↑**: Accelerate forward
- **S/↓**: Brake/Reverse
- **A/←**: Turn left
- **D/→**: Turn right
- **Space**: Drift
- **F**: Activate nitrous (when available)
- **C**: Toggle camera mode
- **R**: Reset vehicle

## Technical Stack

- **Language**: C++20
- **Graphics**: threepp (C++ port of three.js) [1]
- **Audio**: miniaudio [2]
- **Testing**: Catch2 v3 [3]
- **Build System**: CMake

## References

### Primary Libraries & Frameworks

[1] Hjermann, M. (2024). *threepp: C++20 port of three.js (r129)*. GitHub. https://github.com/markaren/threepp

[2] Mackron, D. (2024). *miniaudio: Single file audio playback and capture library*. GitHub. https://github.com/mackron/miniaudio

[3] Catch2 Contributors. (2024). *Catch2 v3: Modern C++ test framework for unit-tests, TDD and BDD*. GitHub. https://github.com/catchorg/Catch2

### Documentation & Technical Resources

[4] three.js Contributors. (2024). *three.js Documentation*. https://threejs.org/docs/

[5] three.js Contributors. (2024). *PerspectiveCamera – three.js docs*. https://threejs.org/docs/#api/en/cameras/PerspectiveCamera

[6] three.js Contributors. (2024). *OrthographicCamera – three.js docs*. https://threejs.org/docs/#api/en/cameras/OrthographicCamera

[7] Wikipedia Contributors. (2024). *Seven-segment display*. Wikipedia. https://en.wikipedia.org/wiki/Seven-segment_display

[8] Stroustrup, B. (2024). *C++ Core Guidelines*. GitHub. https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines

### Game Development & Software Architecture

[9] Nystrom, R. (2014). *Game Programming Patterns*. Genever Benning. https://gameprogrammingpatterns.com/

[10] Red Blob Games. (2024). *Introduction to A* Pathfinding and Game Math*. https://www.redblobgames.com/

[11] Martin, R. C. (2017). *Clean Architecture: A Craftsman's Guide to Software Structure and Design*. Prentice Hall.

[12] MDN Web Docs. (2024). *2D collision detection*. Mozilla Developer Network. https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection

## Project Structure
