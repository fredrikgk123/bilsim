# UML Class Diagram - Bilsimulator

## Simplified Class Structure

```
┌─────────────────────────────────────────────────────────────┐
│                         GameObject                           │
│  (Abstract Base Class)                                       │
├─────────────────────────────────────────────────────────────┤
│ - position_: array<float, 3>                                │
│ - rotation_: float                                           │
│ - size_: array<float, 3>                                    │
│ - active_: bool                                             │
├─────────────────────────────────────────────────────────────┤
│ + update(deltaTime): void {abstract}                        │
│ + reset(): void                                             │
│ + getPosition(): array<float, 3>                            │
│ + getRotation(): float                                      │
│ + checkCircleCollision(...): bool                           │
│ + intersects(other): bool                                   │
└─────────────────────────────────────────────────────────────┘
                            △
           ┌────────────────┼────────────────┐
           │                │                │
           │                │                │
  ┌────────▼─────────┐ ┌───▼────────┐ ┌────▼──────┐
  │    Vehicle       │ │  Obstacle  │ │  Powerup  │
  ├──────────────────┤ ├────────────┤ ├───────────┤
  │ - velocity_      │ │ - type_    │ │ - type_   │
  │ - acceleration_  │ │ - orientation_ │ │         │
  │ - isDrifting_    │ │            │ │           │
  │ - hasNitrous_    │ │            │ │           │
  │ - currentGear_   │ │            │ │           │
  │ - rpm_           │ │            │ │           │
  ├──────────────────┤ ├────────────┤ ├───────────┤
  │ + accelerateForward()│ │ + getType()│ │ + getType()│
  │ + turn()         │ │            │ │           │
  │ + startDrift()   │ │            │ │           │
  │ + activateNitrous()│ │          │ │           │
  │ + update()       │ │ + update() │ │ + update()│
  └──────────────────┘ └────────────┘ └───────────┘


┌─────────────────────────────────────────────────────────────┐
│                      Manager Classes                         │
└─────────────────────────────────────────────────────────────┘

  ┌─────────────────────┐  ┌─────────────────────┐  ┌──────────────────┐
  │  ObstacleManager    │  │  PowerupManager     │  │  SceneManager    │
  ├─────────────────────┤  ├─────────────────────┤  ├──────────────────┤
  │ - obstacles_: vector│  │ - powerups_: vector │  │ - scene_         │
  ├─────────────────────┤  ├─────────────────────┤  │ - camera_        │
  │ + generateWalls()   │  │ + update()          │  │ - renderer_      │
  │ + generateTrees()   │  │ + handleCollisions()│  ├──────────────────┤
  │ + handleCollisions()│  │ + reset()           │  │ + setupLighting()│
  │ + getObstacles()    │  │ + getPowerups()     │  │ + setupGround()  │
  └─────────────────────┘  └─────────────────────┘  │ + updateCamera() │
                                                     │ + render()       │
                                                     └──────────────────┘


┌─────────────────────────────────────────────────────────────┐
│                     Renderer Classes                         │
└─────────────────────────────────────────────────────────────┘

         ┌───────────────────────────┐
         │  GameObjectRenderer       │
         │  (Base Renderer)          │
         ├───────────────────────────┤
         │ # scene_: Scene&          │
         │ # gameObject_: GameObject&│
         │ # objectGroup_: Group     │
         ├───────────────────────────┤
         │ + update(): void          │
         │ # createModel(): void     │
         └───────────────────────────┘
                     △
        ┌────────────┼─────────────┐
        │            │             │
  ┌─────▼────────┐ ┌▼──────────┐ ┌▼─────────────┐
  │VehicleRenderer│ │ObstacleRenderer│ │PowerupRenderer│
  ├──────────────┤ ├────────────┤ ├──────────────┤
  │ - wheelFL_   │ │            │ │              │
  │ - wheelFR_   │ │            │ │              │
  │ - customModel│ │            │ │              │
  ├──────────────┤ ├────────────┤ ├──────────────┤
  │ + loadModel()│ │+ createWall()│ │+ createModel()│
  │ + update()   │ │+ createTree()│ │              │
  └──────────────┘ └────────────┘ └──────────────┘


┌─────────────────────────────────────────────────────────────┐
│                     Support Classes                          │
└─────────────────────────────────────────────────────────────┘

  ┌──────────────────┐  ┌──────────────────┐  ┌─────────────────┐
  │  InputHandler    │  │  AudioManager    │  │  ImGuiLayer     │
  ├──────────────────┤  ├──────────────────┤  ├─────────────────┤
  │ - vehicle_       │  │ - engine_        │  │                 │
  │ - sceneManager_  │  │ - engineSound_   │  │                 │
  │ - keyStates      │  │ - driftSound_    │  │                 │
  ├──────────────────┤  ├──────────────────┤  ├─────────────────┤
  │ + onKeyPressed() │  │ + initialize()   │  │ + render()      │
  │ + onKeyReleased()│  │ + update()       │  │                 │
  │ + update()       │  │                  │  │                 │
  └──────────────────┘  └──────────────────┘  └─────────────────┘


┌─────────────────────────────────────────────────────────────┐
│                     Relationships                            │
└─────────────────────────────────────────────────────────────┘

* GameObject → Vehicle, Obstacle, Powerup (Inheritance)
* GameObjectRenderer → VehicleRenderer, ObstacleRenderer, PowerupRenderer (Inheritance)
* ObstacleManager → Obstacle (Composition, owns obstacles)
* PowerupManager → Powerup (Composition, owns powerups)
* VehicleRenderer → Vehicle (Association, references vehicle)
* InputHandler → Vehicle, SceneManager (Association)
* AudioManager → Vehicle (Association, reads state)
* SceneManager → Renderers (Composition, creates renderers)
```

## Key Design Patterns

1. **Inheritance Hierarchy**
   - GameObject as abstract base
   - Specialized classes: Vehicle, Obstacle, Powerup

2. **Manager Pattern**
   - ObstacleManager: Manages all obstacles
   - PowerupManager: Manages all powerups
   - SceneManager: Manages rendering and scene

3. **Renderer Pattern**
   - Separate rendering from game logic
   - GameObjectRenderer base with specialized renderers
   - Clear separation of concerns

4. **Observer/Callback Pattern**
   - Vehicle reset callback to InputHandler
   - Event-driven input handling

5. **Strategy Pattern**
   - Multiple camera modes (FOLLOW, HOOD, SIDE, INSIDE)
   - Switchable at runtime

## Module Dependencies

```
┌─────────┐
│  core/  │  (No dependencies on other modules)
└────┬────┘
     │
     │ (depends on)
     ▼
┌────────────┐      ┌──────────┐      ┌─────────┐
│ graphics/  │◄─────┤  input/  │◄─────┤   ui/   │
└────────────┘      └──────────┘      └─────────┘
     │                   │
     │                   │ (depends on)
     ▼                   ▼
┌──────────┐      ┌──────────┐
│  audio/  │      │   main   │
└──────────┘      └──────────┘
```

**Principles Demonstrated:**
- High Cohesion: Each module has single responsibility
- Low Coupling: Core independent of presentation layers
- Dependency Inversion: Graphics depends on core abstractions

