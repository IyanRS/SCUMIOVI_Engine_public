#include "CutsceneLoader.hpp"
#include "Cutscene.hpp"
#include "Game.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"
#include "Player.hpp"
#include "json.hpp"

#include <SDL3/SDL.h>
#include <fstream>
#include <filesystem>

using nlohmann::json;

namespace {
	Vector2<> readVec2(const json& j, const std::string& key, Vector2<> def) {
		if (!j.contains(key)) return def;
		const json& arr = j.at(key);
		if (!arr.is_array() || arr.size() < 2) return def;
		return Vector2<>(arr[0].get<float>(), arr[1].get<float>());
	}

	// Players first (they can be anywhere), then the current scene's objects.
	// An object in a different, inactive scene can't be named this way — use
	// the C++ Cutscene builder directly (you'll already have the shared_ptr).
	std::shared_ptr<GameObject> resolveObject(const std::string& name) {
		Game& game = Game::getInstance();
		auto player = game.getPlayerByName(name);
		if (player) return player;
		auto scene = game.getScene();
		if (scene) {
			auto obj = scene->getGameObject(name);
			if (obj) return obj;
		}
		SDL_Log("CutsceneLoader: could not resolve object '%s' (checked the party and the current scene)", name.c_str());
		return nullptr;
	}

	void addStep(Cutscene& cutscene, const json& stepJson);

	void addSteps(Cutscene& cutscene, const json& stepsJson) {
		for (auto& stepJson : stepsJson) {
			addStep(cutscene, stepJson);
		}
	}

	void addStep(Cutscene& cutscene, const json& stepJson) {
		std::string type = stepJson.value("type", std::string());

		if (type == "moveCamera") {
			auto scene = Game::getInstance().getScene();
			if (scene) {
				cutscene.moveCamera(scene->getCamera(), readVec2(stepJson, "position", Vector2<>(0, 0)), stepJson.value("seconds", 1.0f));
			}
			else {
				SDL_Log("CutsceneLoader: moveCamera step has no current scene to get a camera from");
			}
		}
		else if (type == "walk") {
			auto object = resolveObject(stepJson.value("object", std::string()));
			if (object) cutscene.walk(object, readVec2(stepJson, "position", Vector2<>(0, 0)), stepJson.value("seconds", 1.0f));
		}
		else if (type == "walkAtSpeed") {
			auto object = resolveObject(stepJson.value("object", std::string()));
			if (object) cutscene.walkAtSpeed(object, readVec2(stepJson, "position", Vector2<>(0, 0)), stepJson.value("speed", 100.0f));
		}
		else if (type == "say") {
			auto object = resolveObject(stepJson.value("object", std::string()));
			if (object) cutscene.say(object, stepJson.value("text", std::string()), stepJson.value("seconds", 0.0f));
		}
		else if (type == "wait") {
			cutscene.wait(stepJson.value("seconds", 1.0f));
		}
		else if (type == "fadeToBlack") {
			cutscene.fadeToBlack(stepJson.value("seconds", 1.0f));
		}
		else if (type == "fadeFromBlack") {
			cutscene.fadeFromBlack(stepJson.value("seconds", 1.0f));
		}
		else if (type == "changeScene") {
			std::string sceneName = stepJson.value("scene", std::string());
			auto scene = Game::getInstance().findScene(sceneName);
			if (scene) cutscene.changeScene(scene, stepJson.value("fadeDuration", 0.0f));
			else SDL_Log("CutsceneLoader: unknown scene '%s' in changeScene step", sceneName.c_str());
		}
		else if (type == "script") {
			std::string name = stepJson.value("name", std::string());
			cutscene.run([name](EventInfo e) {
				Game::getInstance().scriptEvent(name).execute(e);
			});
			float waitSeconds = stepJson.value("waitSeconds", 0.0f);
			if (waitSeconds > 0.0f) cutscene.wait(waitSeconds);
		}
		else if (type == "parallel") {
			std::vector<Cutscene> branches;
			if (stepJson.contains("branches")) {
				for (auto& branchJson : stepJson.at("branches")) {
					Cutscene branch;
					addSteps(branch, branchJson);
					branches.push_back(branch);
				}
			}
			cutscene.parallel(branches);
		}
		else {
			SDL_Log("CutsceneLoader: unknown step type '%s'", type.c_str());
		}
	}
}

Event CutsceneLoader::loadCutscene(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		SDL_Log("CutsceneLoader: could not open '%s'", path.c_str());
		return Event();
	}

	json data;
	try {
		file >> data;
	}
	catch (const std::exception& ex) {
		SDL_Log("CutsceneLoader: failed to parse '%s': %s", path.c_str(), ex.what());
		return Event();
	}

	Cutscene cutscene;

	if (data.contains("disableMovementFor")) {
		std::string playerName = data.value("disableMovementFor", std::string());
		auto player = Game::getInstance().getPlayerByName(playerName);
		if (player) cutscene.disableMovementFor(player);
		else SDL_Log("CutsceneLoader: unknown player '%s' in disableMovementFor", playerName.c_str());
	}

	if (data.contains("steps")) {
		addSteps(cutscene, data.at("steps"));
	}

	return cutscene.build();
}

bool CutsceneLoader::loadAndRegister(Game& game, const std::string& path, const std::string& scriptName) {
	if (!std::filesystem::exists(path)) {
		SDL_Log("CutsceneLoader: could not open '%s'", path.c_str());
		return false;
	}

	Event cutsceneEvent = loadCutscene(path);
	game.registerScript(scriptName, [cutsceneEvent](EventInfo e) mutable {
		cutsceneEvent.execute(e);
	});
	return true;
}
