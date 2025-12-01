# Bilsimulator 

**Candidate Number:** [YOUR CANDIDATE NUMBER]

> **Note:** AI assistance (GitHub Copilot) was used for audio engine integration, 3D rendering pipeline, and RAII resource management patterns. Core game logic, physics, and collision systems were mostly independently implemented.

---

## What is this?

This is a 3D car simulator built with C++20. The project features a square map on a 2D plane with obstacles and powerups scattered around. The simulator contains simple circle-based collision detection, dynamic steering and a responsive UI. There is no win/lose condition – the focus is on driving and drifting around the map.

Built with:
- **C++20** 
- **threepp** (3D rendering engine)
- **miniaudio** (audio engine for engine/drift sounds)
- **ImGui** (dashboard UI)
- **Catch2** (unit testing framework)

---

## What can it do?

### Key Features

#### Driving Physics
- Realistic car simulation with speed-based steering
- Automatic gear system (1–4 gears + reverse)
- RPM calculation based on speed and gear
- Drift mechanics with drift-angle tracking
- Nitrous boost with time-limited consumption

#### Game Systems
- Collision detection with obstacles, trees and border that stops the car
- Powerup manager with nitrous pickups
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

First of all I am satisfied with making a functional 3D car simulator in C++. The task was quite intimidating at first, considering my little prior knowledge about OOP, but breaking it down into smaller components made it manageable. I like how the simulator look and feels, with responsive controls and a decent driving experience. The integration of audio and UI elements also adds to the overall immersion. I am also very happy with the code structure and organization. Using classes to encapsulate different systems (car, physics, audio, UI) made it easier to manage complexity and keep related functionality together.

I am also very happy with the car model, containing separate parts for body, wheels and steering wheel. This allowed me to implement dynamic wheel rotation and steering animations, which adds a lot to the visual feedback while driving. Also see through windows was a nice touch.


### Areas for improvement

While I'm happy with the end result, there are several things I would do differently if I started over. The collision system works well for what I needed, but it's quite basic with simple circle-based detection. For a larger game with more objects, I'd look into spatial partitioning to improve performance instead of checking every object against every other object.

I also hardcoded a lot of values like speeds and forces directly in the code – using configuration files would make it much easier to tweak and balance the game without recompiling.

Another thing is the delta time handling. I did implement delta time for frame-independent movement, which I'm glad I did, but I learned afterwards that using a fixed timestep for physics would be more stable and predictable. The way it is now, the physics can behave slightly differently depending on frame rate.

Testing was something I added later in the project, and I wish I had started with it from the beginning. Writing tests as I built features would have caught bugs earlier and made me more confident in making changes.

---

## What I learned

This project taught me a lot about structuring a larger C++ program. Coming in with limited OOP experience, I learned how to organize code into logical components and how different systems can communicate with each other. The concept of smart pointers and RAII was completely new to me, but now I understand why manual memory management with raw pointers is considered risky.

I also learned that building something complex doesn't mean doing everything yourself. Using libraries like threepp for rendering and miniaudio for sound let me focus on the actual game logic instead of reinventing the wheel. Knowing when to use existing tools is just as important as knowing how to code.

The debugging process taught me patience and systematic thinking. When something didn't work, I learned to break down the problem into smaller pieces and test each part individually. The unit tests I wrote using Catch2 really helped with this approach.

Overall, this project showed me that even intimidating tasks become manageable when you break them down and tackle them one piece at a time. I'm proud of what I built and excited to apply these skills to future projects.

---

## Screenshots

![Gameplay 1](docs/gameplay.png)
![Gameplay 2](docs/gameplay2.png)

---
