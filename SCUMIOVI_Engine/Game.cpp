#include "Game.hpp"

#include <SDL3/SDL.h>
#include <memory>
#include <algorithm>
#include <fstream>
#include "GameObject.hpp"
#include "Localization.hpp"
#include "json.hpp"

Game::Game() {
	// Initialize standard verbs
	walkVerb = std::make_shared<Verb>("walk", "Walk to");
	pushVerb = std::make_shared<Verb>("push", "Push", VerbSyntax({ "Push", "Push $target"}));
	closeVerb = std::make_shared<Verb>("close", "Close", VerbSyntax({ "Close", "Close $target" }));
	openVerb = std::make_shared<Verb>("open", "Open", VerbSyntax({ "Open", "Open $target" }));
	talkVerb = std::make_shared<Verb>("talk", "Talk to", VerbSyntax({ "Talk to", "Talk to $target" }));
	giveVerb = std::make_shared<Verb>("give", "Give", VerbSyntax({ "Give", "Give $target", "Give $selected to", "Give $selected to $target" }), true);
	pickupVerb = std::make_shared<Verb>("pickup", "Pick up", VerbSyntax({ "Pick up", "Pick up $target" }));
	useVerb = std::make_shared<Verb>("use", "Use", VerbSyntax({ "Use", "Use $target", "Use $selected on $target" }), true);
	lookAtVerb = std::make_shared<Verb>("lookat", "Look at", VerbSyntax({ "Look at", "Look at $target" }));
	pullVerb = std::make_shared<Verb>("pull", "Pull", VerbSyntax({ "Pull", "Pull $target" }));
	anyVerb = std::make_shared<Verb>("any", "Any");
	selectedVerb = std::make_shared<Verb>("selected", "Selected");

	audio.init();

	// Fallback responses used when the game provides no interaction of its own.
	// Games can override these per verb via Verb::setDefaultEvent().
	lookAtVerb->setDefaultEvent(Event([](EventInfo e) {
		auto* go = dynamic_cast<GameObject*>(&e.interactable);
		if (go && !go->getExamineText().empty()) {
			e.player.say(tr(go->getExamineText()));
		} else {
			e.player.say(tr("I don't see anything special."));
		}
	}));
	pushVerb->setDefaultEvent(Event([](EventInfo e) {
		e.player.say(tr("I can't move that."));
	}));
	pullVerb->setDefaultEvent(Event([](EventInfo e) {
		e.player.say(tr("I can't move that."));
	}));

	// Add standard verbs to the verbs vector
	verbs = { walkVerb, pushVerb, closeVerb, openVerb, talkVerb, giveVerb, pickupVerb, useVerb, lookAtVerb, pullVerb };
}

Game* Game::game_ = nullptr;

std::shared_ptr<Player> Game::getCurrentPlayer() {
	return currentPlayer;
}

void Game::setCurrentPlayer(std::shared_ptr<Player> player) {
	currentPlayer = player;
}

Game& Game::getInstance() {
	if (game_ == nullptr)
		game_ = new Game();
	return *game_;
}

void Game::addPlayer(std::shared_ptr<Player> player) {
	players.push_back(player);
}

void Game::removePlayer(std::shared_ptr<Player> player) {
	players.remove(player);
}

std::list<std::shared_ptr<Player>> Game::getPlayers() {
	return players;
}

std::shared_ptr<Player> Game::getPlayerByName(std::string name) {
	for (auto p : players) {
		if (p->getName() == name) {
			return p;
		}
	}
	return nullptr;
}

std::shared_ptr<Verb> Game::getVerb(std::string verbName) {
	for (auto v : verbs) {
		if (v->getName() == verbName) {
			return v;
		}
	}
	return nullptr;
}

void Game::addVerb(std::shared_ptr<Verb> verb) {
	verbs.push_back(verb);
}

void Game::startGame() {
	if (currentScene == nullptr) {
		return;
	}

	startEvent.execute();

	Uint64 perfFreq = SDL_GetPerformanceFrequency();
	Uint64 lastCounter = SDL_GetPerformanceCounter();
	float targetFrameTime = 1.0f / 60.0f;

	while (gameIsRunning) {
		Uint64 currentCounter = SDL_GetPerformanceCounter();
		float deltaTime = (float)(currentCounter - lastCounter) / (float)perfFreq;
		lastCounter = currentCounter;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				gameIsRunning = false;
			}
			inputManager.handleInput(event, ui, *currentScene);
		}
		inputManager.mouseOver(ui, *currentScene);

		currentScene->update(deltaTime);
		Event::tick(deltaTime);
		renderer.updateFade(deltaTime);
		audio.update();
		renderer.render(*currentScene, ui);

		Uint64 afterRenderCounter = SDL_GetPerformanceCounter();
		float frameTime = (float)(afterRenderCounter - lastCounter) / (float)perfFreq;
		float sleepTime = targetFrameTime - frameTime;
		if (sleepTime > 0.001f) {
			SDL_Delay((Uint32)(sleepTime * 1000.0f));
		}
	}

	endEvent.execute();
}

std::shared_ptr<Scene> Game::getScene() {
	return currentScene;
}

void Game::changeScene(std::shared_ptr<Scene> newScene, float fadeDuration) {
	if (fadeDuration > 0.0f && !renderer.isFading()) {
		renderer.fadeToBlack(fadeDuration, Event([this, newScene, fadeDuration]() {
			currentScene = newScene;
			if (currentPlayer) {
				currentPlayer->moveToScene(newScene);
			}
			renderer.fadeFromBlack(fadeDuration);
		}));
	}
	else {
		currentScene = newScene;
		if (currentPlayer) {
			currentPlayer->moveToScene(newScene);
		}
	}
}

UI& Game::getUI() {
	return ui;
}

void Game::setStartEvent(Event event) {
	startEvent = event;
}

void Game::setEndEvent(Event event) {
	endEvent = event;
}

void Game::setDefaultClickEvent(Event event) {
	defaultClickEvent = event;
}

void Game::setCustomCursor(std::shared_ptr<Texture> customCursorTexture, Vector2<> hotspot) {
	renderer.setCustomMouse(customCursorTexture, hotspot);
}

void Game::bindRightClickToVerb(std::shared_ptr<Verb> verb) {
	rightClickVerb = verb;
}

void Game::bindRightClickToVerb(std::string verbName) {
	rightClickVerb = getVerb(verbName);
}

void Game::bindLeftClickToVerb(std::shared_ptr<Verb> verb) {
	leftClickVerb = verb;
}

void Game::bindLeftClickToVerb(std::string verbName) {
	leftClickVerb = getVerb(verbName);
}

std::shared_ptr<Verb> Game::getLeftClickVerb() {
	return leftClickVerb;
}

std::shared_ptr<Verb> Game::getRightClickVerb() {
	return rightClickVerb;
}

std::shared_ptr<Verb> Game::getDefaultVerb() {
	return defaultVerb;
}

void Game::setDefaultVerb(std::shared_ptr<Verb> verb) {
	defaultVerb = verb;
}

void Game::setDefaultVerb(std::string verbName) {
	defaultVerb = getVerb(verbName);
}

void Game::changeToDefaultVerb() {
	if (currentPlayer) {
		currentPlayer->setVerb(defaultVerb);
	}
}

AudioManager& Game::getAudio() {
	return audio;
}

GameState& Game::getGameState() {
	return gameState;
}

void Game::setLocale(const std::string& locale) {
	Localization::getInstance().setLocale(locale);
	// Refresh live UI text (e.g. the current-verb sentence line).
	ui.updateUI();
}

const std::string& Game::getLocale() const {
	return Localization::getInstance().getLocale();
}

void Game::addCombination(const std::string& itemName, const std::string& targetName, Event event) {
	combinations[{itemName, targetName}] = event;
}

Event* Game::findCombination(const std::string& itemName, const std::string& targetName) {
	auto it = combinations.find({itemName, targetName});
	if (it != combinations.end())
		return &it->second;
	return nullptr;
}

// ---------------------------------------------------------------------------
// Named script registry
// ---------------------------------------------------------------------------
void Game::registerScript(const std::string& name, std::function<void(EventInfo)> script) {
	scripts[name] = std::move(script);
}

void Game::registerScript(const std::string& name, std::function<void()> script) {
	scripts[name] = [script = std::move(script)](EventInfo) {
		if (script) script();
	};
}

bool Game::hasScript(const std::string& name) const {
	return scripts.find(name) != scripts.end();
}

Event Game::scriptEvent(const std::string& name) {
	return Event(std::function<void(EventInfo)>([name](EventInfo e) {
		Game& game = Game::getInstance();
		auto it = game.scripts.find(name);
		if (it != game.scripts.end() && it->second)
			it->second(e);
		else
			SDL_Log("Game::scriptEvent: no script registered named '%s'", name.c_str());
	}));
}

Renderer& Game::getRenderer() {
	return renderer;
}

// ---------------------------------------------------------------------------
// Scene registry
// ---------------------------------------------------------------------------
void Game::registerScene(const std::string& name, std::shared_ptr<Scene> scene) {
	scene->setName(name);
	sceneRegistry[name] = scene;
}

std::shared_ptr<Scene> Game::findScene(const std::string& name) {
	auto it = sceneRegistry.find(name);
	if (it != sceneRegistry.end())
		return it->second;
	return nullptr;
}

void Game::clearSceneRegistry() {
	sceneRegistry.clear();
}

// ---------------------------------------------------------------------------
// Inventory prototype registry
// ---------------------------------------------------------------------------
void Game::registerInventoryItemPrototype(const std::string& name, std::shared_ptr<GameObject> prototype) {
	inventoryItemPrototypes[name] = prototype;
}

std::shared_ptr<GameObject> Game::findInventoryItemPrototype(const std::string& name) {
	auto it = inventoryItemPrototypes.find(name);
	if (it != inventoryItemPrototypes.end())
		return it->second;
	return nullptr;
}

// ---------------------------------------------------------------------------
// Save / Load
// ---------------------------------------------------------------------------
std::string Game::getSavePath(int slotIndex) {
	return "save_" + std::to_string(slotIndex) + ".json";
}

bool Game::saveGame(int slotIndex) {
	nlohmann::json data;

	// Game state
	data["gameState"] = gameState.to_json();

	// Current scene
	if (currentScene) {
		data["currentScene"] = currentScene->getName();
	} else {
		data["currentScene"] = "";
	}

	// Player
	if (currentPlayer) {
		nlohmann::json playerData;
		playerData["position"]["x"] = currentPlayer->getPosition().x;
		playerData["position"]["y"] = currentPlayer->getPosition().y;
		playerData["scene"] = currentPlayer->getScene() ? currentPlayer->getScene()->getName() : "";
		if (currentPlayer->getVerb())
			playerData["verb"] = currentPlayer->getVerb()->getName();
		else
			playerData["verb"] = "";

		// Inventory
		nlohmann::json inv = nlohmann::json::array();
		for (auto& item : currentPlayer->getInventory().getItems()) {
			inv.push_back(item->getName());
		}
		playerData["inventory"] = inv;
		data["player"] = playerData;
	}

	// Scene object states
	nlohmann::json scenesData;
	for (auto& [sceneName, scene] : sceneRegistry) {
		nlohmann::json objectsData;
		for (auto& obj : scene->getGameObjects()) {
			nlohmann::json objData;
			objData["state"] = obj->getState();
			objData["visible"] = obj->isVisible();
			objData["position"]["x"] = obj->getPosition().x;
			objData["position"]["y"] = obj->getPosition().y;
			objectsData[obj->getName()] = objData;
		}
		scenesData[sceneName] = objectsData;
	}
	data["scenes"] = scenesData;

	// Write to file
	std::string path = getSavePath(slotIndex);
	std::ofstream file(path);
	if (!file.is_open())
		return false;
	file << data.dump(4);
	file.close();
	return true;
}

bool Game::loadGame(int slotIndex) {
	std::string path = getSavePath(slotIndex);
	std::ifstream file(path);
	if (!file.is_open())
		return false;

	nlohmann::json data;
	try {
		file >> data;
	} catch (...) {
		return false;
	}
	file.close();

	// Game state
	if (data.contains("gameState"))
		gameState.from_json(data["gameState"]);

	// Player
	if (data.contains("player")) {
		auto& playerData = data["player"];
		if (currentPlayer) {
			float x = playerData["position"]["x"].get<float>();
			float y = playerData["position"]["y"].get<float>();
			currentPlayer->setPosition(Vector2<>(x, y));

			if (playerData.contains("verb")) {
				std::string verbName = playerData["verb"].get<std::string>();
				if (!verbName.empty())
					currentPlayer->setVerb(verbName);
			}
		}
	}

	// Current scene
	if (data.contains("currentScene")) {
		std::string sceneName = data["currentScene"].get<std::string>();
		if (!sceneName.empty()) {
			auto scene = findScene(sceneName);
			if (scene) {
				changeScene(scene);
			}
		}
	}

	// Apply object states (must happen after scene objects exist)
	if (data.contains("scenes"))
		applyObjectStates(data["scenes"]);

	// Rebuild inventory
	if (data.contains("player") && data["player"].contains("inventory"))
		applyInventory(data["player"]["inventory"].get<std::vector<std::string>>());

	return true;
}

void Game::applyObjectStates(const nlohmann::json& scenesJson) {
	for (auto& [sceneName, objectsJson] : scenesJson.items()) {
		auto scene = findScene(sceneName);
		if (!scene) continue;
		for (auto& [objName, objData] : objectsJson.items()) {
			auto obj = scene->getGameObject(objName);
			if (!obj) continue;
			if (objData.contains("state"))
				obj->setState(objData["state"].get<int>());
			if (objData.contains("visible"))
				obj->setVisible(objData["visible"].get<bool>());
			if (objData.contains("position")) {
				float x = objData["position"]["x"].get<float>();
				float y = objData["position"]["y"].get<float>();
				obj->setPosition(Vector2<>(x, y));
			}
		}
	}
}

void Game::applyInventory(const std::vector<std::string>& itemNames) {
	if (!currentPlayer) return;
	auto& inventory = currentPlayer->getInventory();
	inventory.getItems().clear();
	for (auto& name : itemNames) {
		auto proto = findInventoryItemPrototype(name);
		if (proto) {
			auto clone = std::make_shared<GameObject>(name, Vector2<>(), Vector2<>(-1, -1), proto->getTexture());
			clone->addTag("item");
			inventory.addItem(clone);
		}
	}
}

void Game::quitGame() {
	gameIsRunning = false;
}