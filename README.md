# Bilsimulator 

**Candidate Number:** [YOUR CANDIDATE NUMBER]

> **Note:** AI assistance (GitHub Copilot) was used for audio engine integration, 3D rendering pipeline, and RAII resource management patterns. Core game logic, physics, and collision systems were independently implemented.

---

## What is this?

This is a 3D car simulator built with C++20. The project features a square map on a 2D plane with obstacles and powerups scattered around. The simulator contains simple circle-based collision detection, dynamic steering and a responsive UI.

Built with:
- **C++20** 
- **threepp** (3D rendering engine)
- **miniaudio** (audio engine for engine/drift sounds)
- **ImGui** (dashboard UI and debugging tools)
- **Catch2** (unit testing framework)

---

## What can it do?

### Key Features

#### Driving Physics
- Realistic car simulation with speed-based steering
- Automatic gear system (1–5 gears + reverse)
- RPM calculation based on speed and gear
- Drift mechanics with drift-angle tracking
- Nitrous boost with time-limited consumption

#### Game Systems
- Collision detection with obstacles, trees and border that stops the car
- Powerup manager with nitrous pickups that respawn
- Respawn system that resets the car to the spawn point and respawns powerups
- Multiple camera angles (follow, interior)

#### Graphics
- 3D models loaded from OBJ files (car body, wheels, steering wheel)
- Dynamic camera with field-of-view changes at high speed or while using nitrous
- Minimap with top-down view in the top-left corner
- ImGui dashboard showing speed, gear, RPM and nitrous status

#### Audio
- RPM-based engine sound with dynamic pitch and volume
- Tire screech audio when drifting above a threshold speed
- Nitrous boost sound effects with pitch/volume adjustments

---

## How do I use it?

### Build

```bash
git clone [repository-url]
cd bilsim
mkdir build && cd build
cmake ..
cmake --build .
# or reuse the configured directory
cmake --build ../cmake-build-debug-cmake --target bilsim
```

### Run

```bash
./cmake-build-debug-cmake/src/bilsim
```

### Test

```bash
cd cmake-build-debug-cmake
ctest --output-on-failure
```

### Controls

| Key | Action                              |
|-----|-------------------------------------|
| **W** | Accelerate forward                  |
| **S** | Brake / reverse                     |
| **A** | Turn left                           |
| **D** | Turn right                          |
| **Space** | Handbrake, this starts drift        |
| **F** | Use nitrous boost (requires pickup) |
| **C** | Switch camera mode                  |
| **Arrow Keys** | Adjust camera look direction        |
| **R** | Respawn (reset car position)        |
| **ESC** | Exit game                           |

---
### Simplified UML Diagram

![UML Diagram](docs/umldiagram.png)

## Project Reflection

### What I'm satisfied with

[FILL IN – e.g., modern C++ practices, modular design, physics fidelity, audio integration, tests]

### Areas for improvement

[FILL IN – e.g., delta-time clamping, fixed timestep, spatial partitioning, config files]

---

## What I learned

[FILL IN – summarize key takeaways]

---

## Screenshots

![Gameplay 1](docs/gameplay.png)
![Gameplay 2](docs/gameplay2.png)

---

