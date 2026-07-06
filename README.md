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
| `game/`            | Data definitions for the sample game: `Verbs.csv`, `Objects.csv`, `Players.csv`, `Scenes/`, `Dialogues/`, `Interactions.csv`, `Combinations.csv`, `TextFormats.csv`, `Settings.csv`. |
| `codegen/`         | Python + Jinja2 generator that turns `game/*.csv` into `SCUMIOVI_Game/GameData.cpp`. |
| `assets/`          | Textures, fonts, locale files (`assets/locales/`) and UI layouts (`assets/ui/`) loaded at runtime. |
| `tests/`           | Catch2 unit tests. |
| `third_party/`     | Vendored single-header libraries (`json.hpp`, `catch.hpp`). |
| `SDL3/`, `SDL3_image/`, `SDL3_ttf/` | Third-party SDL libraries (headers + prebuilt Windows binaries). |

## Architecture overview

The engine draws a hard line between itself and any particular game: `Game`'s
constructor creates only the "any"/"selected" protocol verbs the interaction
system depends on structurally — every game-facing verb (walk, use, give, ...),
scene, object, dialogue line and UI layout comes from `game/*.csv`,
`game/Dialogues/*.txt` or `assets/ui/*.json`, loaded by the generated
`GameData.cpp` at startup. The only thing that's still C++ by design is
*custom scripts* (`SCUMIOVI_Game/Interactions/*.cpp`) — bespoke behavior a CSV
can't express, referenced from data by name (see `Game::registerScript`).

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
- **`Game::registerScript`/`scriptEvent`** is a named-script registry: object
  interactions, dialogue actions, combinations and UI actions can all reference
  a developer-provided script by name instead of only by inline lambda.
- **`UIElement`** is a `GameObject` with a parent-relative offset, a nine-point
  anchor and child elements; `UILayout` flattens that tree into an
  absolute-positioned draw/hit-test list each frame. See "Data-driven UI" below.
- **`Localization`** is a gettext-style catalogue (`tr(sourceString)`); see
  "Localization" below.

Global configuration (window/render size, target FPS, pixel-perfect toggles)
lives in `SCUMIOVI_Engine/Common.hpp`.

## Localization

User-facing strings are written in English directly in engine code and
generated game data, and that string doubles as the lookup key — so English
needs no data file, and any string missing from another locale's file falls
back to English instead of showing a blank or a raw key.

```cpp
tr("I don't see anything special.");     // -> looked up in the active locale
Game::getInstance().setLocale("es");     // switches at runtime
```

Translations live in `assets/locales/<locale>.json` as flat
`{ "source string": "translation" }` maps (see `assets/locales/es.json`) and
are loaded with `Localization::loadLocaleFile`. `Verb` display names/sentence
templates, `DialogueOption` text, and JSON-authored UI labels/buttons (see
below) all resolve through `tr()` on every refresh, so a locale switch updates
already-visible text without rebuilding anything.

## Data-driven UI

Menus, HUDs and inventories are authored as JSON under `assets/ui/` and loaded
by `UILoader` — no C++ required for a new menu. `SCUMIOVI_Game`'s own HUD
(`assets/ui/inventory.json`, `assets/ui/dialogue.json`) is itself built this
way; the codegen just calls
`UILoader::loadLayoutsFromDirectory(game, ResourceManager::getUIDirectory())`.

```json
{
  "name": "options",
  "elements": [
    { "type": "label", "text": "Options", "anchor": "top-center" },
    { "type": "button", "text": "Language",
      "anchor": "center", "onClick": { "action": "script", "name": "cycleLanguage" } },
    { "type": "button", "text": "Back",
      "anchor": "bottom-center", "onClick": { "action": "closeOverlay" } }
  ]
}
```

Element `type`s: `image`, `label`, `button`, `panel` (container, nestable via
`"children"`), `infoLabel` (self-refreshing current-verb sentence), and the
top-level-only `verbGrid`/`inventoryGrid` (built through
`UILayout::createVerbsMatrix`/`createInventoryMatrix`, which own the label-width
column sizing and slot geometry). Common fields: `position`/`size` ([x, y]),
`anchor` (nine-point: `top-left` … `bottom-right`), `texture` (an image
filename or an `[r, g, b, a]` solid color), and `text*` formatting fields
(`textSize`, `textColor`, `textOutline`, `textShadow`, `textAlign`).

`onClick`/`onHover` accept either a bare string (shorthand for
`{"action": "script", "name": "<string>"}`, resolving through
`Game::registerScript`) or an action object from a small built-in vocabulary:
`script`, `openLayout`, `openOverlay` (renders on top of the current layout and
optionally pauses the scene — see `UI::openOverlay`), `closeOverlay`,
`setLocale`, `setVerb`, `save`, `load`, `quit`.

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
