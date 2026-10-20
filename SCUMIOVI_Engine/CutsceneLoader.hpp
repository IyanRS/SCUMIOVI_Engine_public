#pragma once

#include "Event.hpp"
#include <string>

class Game;

// -----------------------------------------------------------------------------
// CutsceneLoader
//
// Parses a JSON file into an Event, using the same step vocabulary as the
// Cutscene C++ builder (see Cutscene.hpp) — a game can author simple linear
// (or forked/joined) sequences as data instead of writing C++, and drop into
// a named script (see Game::registerScript) for anything the built-in step
// types can't express.
//
// {
//   "name": "intro_cutscene",
//   "disableMovementFor": "hero",
//   "steps": [
//     { "type": "moveCamera", "position": [100, 50], "seconds": 2.0 },
//     { "type": "walk", "object": "hero", "position": [120, 80], "seconds": 3.0 },
//     { "type": "say", "object": "hero", "text": "Where's the map?" },
//     { "type": "wait", "seconds": 1.0 },
//     { "type": "fadeToBlack", "seconds": 1.0 },
//     { "type": "changeScene", "scene": "nextRoom", "fadeDuration": 0.5 },
//     { "type": "fadeFromBlack", "seconds": 1.0 },
//     { "type": "script", "name": "someCustomStep", "waitSeconds": 0.5 },
//     { "type": "parallel", "branches": [
//         [ { "type": "walk", "object": "merchant", "position": [10, 10], "seconds": 1.5 } ],
//         [ { "type": "fadeToBlack", "seconds": 1.0 } ]
//     ] }
//   ]
// }
//
// "object" names are resolved against the party (by player name) first, then
// the current scene's objects; "scene" names are resolved via Game::findScene.
// A "script" step runs a Game::registerScript'd action and, if "waitSeconds"
// is given, waits that long before continuing (there's no way for a named
// script to signal "I'm done" on its own timeline — for that, build the
// cutscene with the C++ Cutscene API directly).
// -----------------------------------------------------------------------------
class CutsceneLoader {
public:
	// Parses a single cutscene file into an Event. Returns an empty (no-op)
	// Event, and logs, on a missing file or invalid JSON.
	static Event loadCutscene(const std::string& path);

	// Loads a cutscene and registers it as a named script (see
	// Game::registerScript), so object interactions, dialogue actions and UI
	// actions can trigger it purely by name like any other script. Returns
	// false if the file doesn't exist.
	static bool loadAndRegister(Game& game, const std::string& path, const std::string& scriptName);
};
