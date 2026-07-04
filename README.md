# SCUMIOVI Engine

A 2D point-and-click adventure game engine in the classic **SCUMM** style
(*Maniac Mansion*, *Monkey Island*), written in modern C++20 on top of
[SDL3](https://www.libsdl.org/). The engine provides the building blocks of a
graphic adventure — a verb/interaction system, walkable areas with pathfinding,
an inventory, branching dialogue, animated actors, scene management and
save/load — while game content is described in data and generated into C++.

This repository contains the engine, a sample game (a small *Monkey Island*
inspired scene), a data-driven code generator, and a unit-test suite.

## Repository layout

| Path | Description |
|------|-------------|
| `SCUMIOVI_Engine/` | The engine library (rendering, input, audio, scenes, game objects, dialogue, UI, save/load). |
| `SCUMIOVI_Game/`   | The sample game. `GameData.cpp` here is **generated** from the CSV files in `game/`. |
| `game/`            | Data definitions for the sample game (scenes, objects, animations, dialogue, UI, verbs, text formats). |
| `codegen/`         | Python + Jinja2 generator that turns `game/*.csv` into `SCUMIOVI_Game/GameData.cpp`. |
| `assets/`          | Textures and fonts loaded at runtime. |
| `tests/`           | Catch2 unit tests. |
| `third_party/`     | Vendored single-header libraries (`json.hpp`, `catch.hpp`). |
| `SDL3/`, `SDL3_image/`, `SDL3_ttf/` | Third-party SDL libraries (headers + prebuilt Windows binaries). |

## Architecture overview

The engine is organised around a **component-based `GameObject`** and a set of
focused managers, coordinated by a `Game` singleton:

- **`GameObject`** composes `MovementComponent`, `AnimationComponent`,
  `GraphicsComponent` and `TextComponent`, and derives from `Interactable`
  (verb interactions, mouse-over/click handling).
- **`Scene`** owns the game objects, the `Camera`, the walkable area, and runs
  per-frame simulation (`update`), including overlap-event detection.
- **`Renderer`** is responsible for drawing only; it reads game state and does
  not mutate the simulation.
- **`InputManager`** translates SDL mouse/keyboard events into verb interactions.
- **`Event`** is a list of actions (with optional `Delay`s) executed through a
  non-blocking, frame-driven pending queue rather than blocking threads.
- **`GameState`**, `saveGame`/`loadGame` persist progress to JSON.

Global configuration (window/render size, target FPS, pixel-perfect toggles)
lives in `SCUMIOVI_Engine/Common.hpp`.

## Building

### Engine + sample game (Visual Studio)

Requires **Visual Studio 2022** with the C++ desktop workload (C++20).

1. Open `SCUMIOVI_Engine.sln`.
2. Select the `x64` configuration.
3. Build and run the `SCUMIOVI_Game` project.

In-game, **F5** quick-saves to slot 0 and **F9** quick-loads it.

### Tests

`build_tests.bat` compiles the engine sources together with the Catch2 test
suite and runs them. It uses the MSVC toolchain, so run it from any shell with
Visual Studio 2022 installed:

```bat
build_tests.bat
```

The script initialises the MSVC environment via `vcvarsall.bat`, builds into
`tests/build/`, and executes `run_tests.exe`.

## Regenerating game data

`SCUMIOVI_Game/GameData.cpp` is auto-generated — do not edit it by hand. To
change the sample game, edit the CSV files under `game/` and regenerate:

```bash
cd codegen/templates
python generate.py
```

This requires Python 3 with Jinja2 (`pip install jinja2`).

## Dependencies

- [SDL3](https://www.libsdl.org/) — windowing, rendering, input, audio.
- SDL3_image — texture loading.
- SDL3_ttf — text rendering.
- [nlohmann/json](https://github.com/nlohmann/json) — save/load serialisation (single-header, vendored).
- [Catch2](https://github.com/catchorg/Catch2) — unit testing (single-header, vendored).
