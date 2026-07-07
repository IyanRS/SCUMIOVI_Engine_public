#include "KeyBindings.hpp"
#include "UIAction.hpp"
#include "json.hpp"

#include <fstream>

bool KeyBindings::bind(const std::string& keyName, Event action) {
	SDL_Keycode key = SDL_GetKeyFromName(keyName.c_str());
	if (key == SDLK_UNKNOWN) {
		SDL_Log("KeyBindings: unrecognized key name '%s'", keyName.c_str());
		return false;
	}
	bindings[key] = std::move(action);
	return true;
}

bool KeyBindings::loadFromFile(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		SDL_Log("KeyBindings: could not open '%s'", path.c_str());
		return false;
	}

	nlohmann::json data;
	try {
		file >> data;
	}
	catch (const std::exception& ex) {
		SDL_Log("KeyBindings: failed to parse '%s': %s", path.c_str(), ex.what());
		return false;
	}

	if (!data.is_object()) {
		SDL_Log("KeyBindings: '%s' must be a JSON object of key:action pairs", path.c_str());
		return false;
	}

	bool anyBound = false;
	for (auto& [keyName, actionJson] : data.items()) {
		if (bind(keyName, resolveUIAction(actionJson))) {
			anyBound = true;
		}
	}
	return anyBound;
}

bool KeyBindings::hasBinding(SDL_Keycode key) const {
	return bindings.find(key) != bindings.end();
}

bool KeyBindings::handleKeyDown(SDL_Keycode key) {
	auto it = bindings.find(key);
	if (it == bindings.end()) {
		return false;
	}
	it->second.execute();
	return true;
}
