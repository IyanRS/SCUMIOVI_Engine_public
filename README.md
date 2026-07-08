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
| `assets/`          | Textures, fonts, locale files (`assets/locales/`), UI layouts (`assets/ui/`) and `keybindings.json`, all loaded at runtime. |
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
`Game::registerScript`) or an action object from a small built-in vocabulary
(`UIAction.hpp`, shared with key bindings — see below): `script`, `openLayout`,
`openOverlay` (renders on top of the current layout and optionally pauses the
scene — see `UI::openOverlay`), `toggleOverlay` (opens it, or closes it if it's
already the active overlay — what a single key like Escape wants), `closeOverlay`,
`setLocale`, `setVerb`, `switchPlayer`/`switchToNextPlayer`/`switchToPreviousPlayer`
(see "Multi-character parties" below), `save`, `load`, `quit`.

## Multi-character parties

Classic multi-character SCUMM games (Day of the Tentacle, Maniac Mansion) let
you control several characters that can be off in different rooms
simultaneously. The engine supports this out of the box, even though the
sample game only uses one character:

```cpp
game.addPlayer(hoagie);
game.addPlayer(laverne);
game.switchToPlayer(laverne);          // or switchToPlayer("laverne")
game.switchToNextPlayer(0.5f);         // cycles party order, fades over 0.5s
```

`Game::switchToPlayer` makes the given (already-added) player active. If that
player is in a different scene than the one currently showing, the active
scene switches to follow them there — each `Player` already tracks its own
verb selection, selected/target object and inventory independently, so
switching back and forth preserves per-character state automatically.
`switchToNextPlayer`/`switchToPreviousPlayer` cycle through the party in
`addPlayer` order. All four are also available as `onClick`/key-binding
actions (`switchPlayer`, `switchToNextPlayer`, `switchToPreviousPlayer`), so a
game can wire up character-portrait buttons or a hotkey entirely from data.

Camera follow behavior is intentionally **not** changed automatically by a
switch — a game using `Camera::setObjectToFollow` should re-point it itself
if it wants the camera to track whichever character is currently active.

`saveGame`/`loadGame` persist **every** party member's own position, scene,
verb and inventory (keyed by player name), not just whoever is active at save
time — so saving mid-game doesn't lose a character who's off in a different
room. Loading also cancels each restored player's in-progress action (walking,
a one-shot animation, mid-speech text — see `GameObject::stopCurrentAction`)
before repositioning them, so a character who was mid-walk when you load an
older save lands exactly at the saved position instead of continuing to
interpolate toward wherever they were headed before the load.

## Talking animation

`GameObject::setTalkAnimations(right, left, up, down)` registers directional
animations that play automatically for the duration of `say()`/`Player::say()`
and revert to idle when the text ends (or is cut short by
`stopCurrentAction()`), mirroring how `setMovementAnimations` already handles
walk/idle. Games that don't call it are unaffected — `say()` just doesn't
touch the animation, exactly as before.

```cpp
guybrush->setTalkAnimations(talkRight, talkLeft, nullptr, nullptr);
guybrush->say("I'm looking for a three-headed monkey.");
```

## Cutscenes

Scripted, non-interactive sequences (camera pans, actor walks, dialogue,
fades, scene changes) are built with `Cutscene`, a fluent builder that
produces an ordinary `Event` — it doesn't introduce a new execution model,
just automates the callback-nesting a sequence like this would otherwise
require by hand:

```cpp
Cutscene()
    .moveCamera(scene->getCamera(), pos, 2.0f)
    .walk(guybrush, doorPos, 3.0f)
    .say(guybrush, "Where's the map?")
    .parallel({
        Cutscene().walk(stan, spot1, 1.5f),
        Cutscene().fadeToBlack(1.0f)
    })
    .changeScene(nextScene, 0.5f)
    .disableMovementFor(guybrush)   // re-enabled automatically when it ends
    .build();                       // -> Event
```

`.parallel()` runs several sub-sequences concurrently and only continues once
every branch finishes. `Cutscene::requestSkip()` (wired to the `skipCutscene`
UI/key-binding action) jumps a running cutscene straight to its end at the
next step boundary — it won't cut a step already in progress short, but skips
everything queued after it.

The same step vocabulary is available as **data** via `CutsceneLoader`,
mirroring how dialogue and UI are authored:

```json
{
  "name": "intro_cutscene",
  "disableMovementFor": "guybrush",
  "steps": [
    { "type": "walk", "object": "guybrush", "position": [120, 80], "seconds": 3.0 },
    { "type": "say", "object": "guybrush", "text": "Where's the map?" },
    { "type": "fadeToBlack", "seconds": 1.0 },
    { "type": "changeScene", "scene": "nextRoom", "fadeDuration": 0.5 },
    { "type": "script", "name": "someCustomEffect", "waitSeconds": 0.5 }
  ]
}
```

`object`/`scene` names resolve against the party and the current scene (see
`CutsceneLoader.hpp` for the full step list, including `parallel`).
`CutsceneLoader::loadAndRegister(game, path, name)` loads a file and registers
it as a named script in one call, so it can be triggered from an object
interaction or dialogue action exactly like any other script. A `script` step
runs a `Game::registerScript`'d action and, since a plain script has no way to
report "I'm done," an optional `waitSeconds` covers fixed-duration custom
effects — a script whose duration isn't known up front should be composed
with the C++ `Cutscene` builder instead.

## Key bindings

What each key does is game data, not an engine `switch` statement. `assets/keybindings.json`
maps SDL key names (`SDL_GetKeyFromName`, e.g. `"Escape"`, `"F5"`, `"A"`) to the
same action vocabulary UI buttons use:

```json
{
  "Escape": { "action": "toggleOverlay", "name": "pauseMenu" },
  "F5": { "action": "save", "slot": 0 },
  "F9": { "action": "load", "slot": 0 }
}
```

Loaded once via `KeyBindings::loadFromFile` (called from the generated
`loadGame()`) into `Game::getInstance().getInputManager().getKeyBindings()`.
The sample game's `assets/ui/pause.json` (Resume / Save-Load / Quit) and
`assets/ui/saveload.json` (3 save slots) show the overlay pattern this enables:
Escape pauses the scene and shows a menu built entirely from data.

## Sample game

`SCUMIOVI_Game` (the `game/` + `assets/` data driving it) is a small,
deliberately minimal demo whose only job is to exercise every engine system
end-to-end:

- **Verbs**: all ten verbs from `Verbs.csv` are wired to at least one object
  (`push`/`pull`/`open`/`close`/`lookat` were previously defined but unused).
- **Examine text**: objects carry an `examine_text` column (`Objects.csv`)
  resolved automatically by the `lookat` verb's default event.
- **Dialogue with state**: `game/Dialogues/stan.txt` branches on a flag
  (`if:gave_mug_to_stan`) set by the `give_mug_stan` interaction script.
- **Item combinations**: `mug` used on `table` (`Combinations.csv`).
- **Localization**: the pause menu has EN/ES buttons wired to `setLocale`.
- **Multi-character party**: a second player, `otis`, is registered
  (reusing Stan's existing sprite as a placeholder — no new art). **Tab**
  switches the active party member via `switchToNextPlayer`.
- **A second room**: `game/Scenes/backroom/` plus a `door_to_backroom` /
  `door_to_bar` pair of objects (`open` verb) demonstrate `changeScene` with
  a fade between two registered scenes.
- **A cutscene**: `assets/cutscenes/intro.json` plays once at startup via
  `CutsceneLoader`, showcasing camera movement, `say`, and fades together.
  **Space** skips it (`skipCutscene`).

**Placeholder assets you'll need to draw in:** `assets/textures/door.png`
(used by both doors) and the `backroom.png` background referenced by
`game/Scenes/backroom/Background.csv`. Missing textures are logged and
skipped rather than crashing (`ResourceManager::loadTexture`), so the demo
runs today with those two spots simply blank until real art is dropped in.

## Building

### Engine + sample game (Visual Studio)

Requires **Visual Studio 2022** with the C++ desktop workload (C++20).

1. Open `SCUMIOVI_Engine.sln`.
2. Select the `x64` configuration.
3. Build and run the `SCUMIOVI_Game` project.

In-game, **Escape** opens the pause menu (Resume / Save-Load / Quit), and
**F5**/**F9** quick-save/quick-load slot 0 — all three are data (see "Key
bindings" below), not hardcoded engine behavior.

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
