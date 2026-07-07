#pragma once

#include "Event.hpp"

#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

// -----------------------------------------------------------------------------
// KeyBindings
//
// Maps keyboard keys to Events, so a game can say "Escape opens the pause
// menu" as data (see assets/keybindings.json) instead of the engine hardcoding
// a switch over specific SDLK_* values. Bound actions come from the same
// vocabulary as UI onClick/onHover (see UIAction.hpp) — a binding is just an
// action descriptor keyed by an SDL key name (e.g. "Escape", "F5", "A").
// -----------------------------------------------------------------------------
class KeyBindings {
	std::unordered_map<SDL_Keycode, Event> bindings;

public:
	// Binds a single key (looked up via SDL_GetKeyFromName) to an action,
	// replacing any existing binding for that key. Returns false (and logs) if
	// the key name isn't recognized by SDL.
	bool bind(const std::string& keyName, Event action);

	// Loads a JSON file of { "KeyName": <action descriptor>, ... } (the action
	// descriptor shape is resolved via resolveUIAction). Returns false (and
	// logs) on a missing file, invalid JSON, or if every entry failed to bind.
	bool loadFromFile(const std::string& path);

	bool hasBinding(SDL_Keycode key) const;

	// Runs the bound action for this key, if any (via Event::execute(), the
	// same no-click-context path Event already uses for non-UI events).
	// Returns true if a binding existed and was run.
	bool handleKeyDown(SDL_Keycode key);
};
