#pragma once

#include <string>

class Game;

// -----------------------------------------------------------------------------
// UILoader
//
// Builds UILayouts from JSON files, so game developers can author menus,
// inventories and HUDs as data instead of C++. Buttons and other interactive
// elements reference named scripts (see Game::registerScript) or a small
// built-in action vocabulary (script, openLayout, openOverlay, closeOverlay,
// setLocale, setVerb, save, load, quit) through their "onClick"/"onHover"
// fields. Text fields are resolved through the localization system and kept
// in sync with locale switches.
//
// See assets/ui/*.json for authored examples of the schema.
// -----------------------------------------------------------------------------
class UILoader {
public:
	// Parses a single layout file and registers it with game.getUI() under the
	// name given by its top-level "name" field. Returns false (and logs) on a
	// missing file, invalid JSON, or a missing/empty "name".
	static bool loadLayout(Game& game, const std::string& path);

	// Loads every top-level *.json file in a directory as a layout.
	// Returns false if the directory doesn't exist or any file failed to load.
	static bool loadLayoutsFromDirectory(Game& game, const std::string& directoryPath);
};
