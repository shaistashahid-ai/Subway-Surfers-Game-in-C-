# MetroSprint: OOP Runner

**MetroSprint** is a 2D, three-lane infinite runner game inspired by Subway Surfers.  
This project is designed to be a programming assignment / portfolio piece that demonstrates strong object-oriented design using encapsulation, inheritance, abstraction, polymorphism, composition, and templates/generics.

---

## Features
- Three discrete lanes (Left, Middle, Right).
- Player auto-runs forward. Controls:
  - Move left: `A`
  - Move right: `D`
  - Jump: `W`
  - Slide: `S`
- State-based player actions to prevent conflicting inputs (idle, running, jumping, sliding).
- Obstacle hierarchy (abstract `Obstacle` base class with concrete `Train`, `Barrier`, `Cone`, `Fence`) — each with custom collision behaviors.
- Coins to collect (50 points per coin).
- Time-bound power-ups implemented with an abstract `PowerUp` class and specific types:
  - `MagnetPower` (attracts coins)
  - `JetpackPower` (fly over obstacles)
  - `ShieldPower` (one collision protection)
  - `DoubleCoinPower` (coins worth double)
- Dynamic difficulty: game speed and spawn rates increase as the game progresses.
- Scoring:
  - 10 points per second survived
  - 50 points per coin
  - Extra/bonus points from power-ups
- Game over when player collides with an obstacle without an active shield.

---

## Design Overview (OOP focus)
- **Player**  
  - Fields: `lane`, `position`, `speed`, `state`, `activePowerUps`  
  - Methods: `moveLeft()`, `moveRight()`, `jump()`, `slide()`, `update(dt)`, `applyPowerUp(p)`
  - Uses a **State** pattern to manage `Idle`, `Running`, `Jumping`, `Sliding`.

- **Obstacle** (abstract base)  
  - Fields: `lane`, `position`, `size`  
  - Methods: `update(dt)`, `onCollision(player)` — overridden by each concrete obstacle.

  - Concrete classes: `Train`, `Barrier`, `Cone`, `Fence` — each implements `onCollision` differently (e.g. high obstacle requires slide, low obstacle requires jump, train is instant death unless shield).

- **PowerUp** (abstract base)  
  - Fields: `duration`, `activatedAt`, `lane`, `position`  
  - Methods: `apply(player)`, `expire(player)`, `update(dt)`  
  - Concrete classes: `MagnetPower`, `JetpackPower`, `ShieldPower`, `DoubleCoinPower`.

- **Coin**  
  - Fields: `lane`, `position`  
  - Behavior: collects when player overlaps; adds to score (modified by `DoubleCoinPower`).

- **Spawner / GameManager**  
  - Handles timed spawns of obstacles, coins, and power-ups.
  - Adjusts spawn rates based on `gameSpeed`.
  - Central game loop updating all entities, checking collisions, updating score and speed.

- **Collision System**  
  - Lane + vertical state checks to determine collision outcomes.
  - Example: `Barrier` collides if player is not sliding; `Cone` collides if player not jumping.

- **Utility / Templates**  
  - Use templates/generics for factory functions or strongly-typed containers (e.g., `EntityFactory<T>` or `Pool<T>`).


