#include "Game.hpp"

#include <SDL3/SDL.h>
#include <memory>
#include <algorithm>
#include <fstream>
#include "GameObject.hpp"
#include "Localization.hpp"
#include "json.hpp"

Game::Game() {
	// "any" and "selected" are protocol verbs the interaction system itself
	// depends on structurally (Interactable::onClick, UI element click binding),
	// not game content — every other verb (walk, use, give, ...) is defined by
	// the game's own data (see game/Verbs.csv) and created during loadGame().
	anyVerb = std::make_shared<Verb>("any", "Any");
	selectedVerb = std::make_shared<Verb>("selected", "Selected");

	audio.init();
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

// ---------------------------------------------------------------------------
// Multi-character party switching
// ---------------------------------------------------------------------------
bool Game::switchToPlayer(std::shared_ptr<Player> player, float fadeDuration) {
	if (!player) return false;
	if (std::find(players.begin(), players.end(), player) == players.end()) return false;

	auto targetScene = player->getScene();

	// Unlike changeScene(), we don't call moveToScene() here: the player being
	// switched to is already in targetScene (they may have been there all
	// along), we're just making the game follow them there.
	auto applySwitch = [this, player, targetScene]() {
		currentPlayer = player;
		if (targetScene) {
			currentScene = targetScene;
		}
		ui.updateUI();
	};

	bool changingScene = targetScene && targetScene != currentScene;
	if (fadeDuration > 0.0f && changingScene && !renderer.isFading()) {
		renderer.fadeToBlack(fadeDuration, Event([this, applySwitch, fadeDuration]() {
			applySwitch();
			renderer.fadeFromBlack(fadeDuration);
		}));
	}
	else {
		applySwitch();
	}
	return true;
}

bool Game::switchToPlayer(const std::string& name, float fadeDuration) {
	return switchToPlayer(getPlayerByName(name), fadeDuration);
}

bool Game::switchToNextPlayer(float fadeDuration) {
	if (players.size() < 2) return false;
	auto it = std::find(players.begin(), players.end(), currentPlayer);
	if (it == players.end()) return false;
	++it;
	if (it == players.end()) it = players.begin();
	return switchToPlayer(*it, fadeDuration);
}

bool Game::switchToPreviousPlayer(float fadeDuration) {
	if (players.size() < 2) return false;
	auto it = std::find(players.begin(), players.end(), currentPlayer);
	if (it == players.end()) return false;
	if (it == players.begin()) it = players.end();
	--it;
	return switchToPlayer(*it, fadeDuration);
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

		// A pausing overlay (e.g. a pause/options menu) freezes the scene
		// simulation while it's shown; Event::tick still runs so the overlay's
		// own UI (fades, delayed actions) keeps working.
		if (!ui.isGamePaused()) {
			currentScene->update(deltaTime);
		}
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

void Game::changeScene(std::shared_ptr<Scene> newScene, float fadeDuration, Event callback) {
	if (fadeDuration > 0.0f && !renderer.isFading()) {
		renderer.fadeToBlack(fadeDuration, Event([this, newScene, fadeDuration, callback]() {
			currentScene = newScene;
			if (currentPlayer) {
				currentPlayer->moveToScene(newScene);
			}
			renderer.fadeFromBlack(fadeDuration, callback);
		}));
	}
	else {
		currentScene = newScene;
		if (currentPlayer) {
			currentPlayer->moveToScene(newScene);
		}
		callback.execute();
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

InputManager& Game::getInputManager() {
	return inputManager;
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
	data["currentPlayerName"] = currentPlayer ? currentPlayer->getName() : "";

	// Every party member's own position/scene/verb/inventory is saved — not
	// just whoever is currently active — so multi-character games (where
	// party members can be off in different rooms simultaneously; see
	// Game::switchToPlayer) don't lose anyone on load.
	nlohmann::json playersData;
	for (auto& player : players) {
		nlohmann::json playerData;
		playerData["position"]["x"] = player->getPosition().x;
		playerData["position"]["y"] = player->getPosition().y;
		playerData["scene"] = player->getScene() ? player->getScene()->getName() : "";
		playerData["verb"] = player->getVerb() ? player->getVerb()->getName() : "";

		nlohmann::json inv = nlohmann::json::array();
		for (auto& item : player->getInventory().getItems()) {
			inv.push_back(item->getName());
		}
		playerData["inventory"] = inv;

		playersData[player->getName()] = playerData;
	}
	data["players"] = playersData;

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

	// Restore every party member's own position/scene/verb/inventory. Any
	// in-progress action (walking, a one-shot animation, mid-speech text) is
	// cancelled first — otherwise a character mid-walk when the load happens
	// would keep interpolating toward its old (pre-load) destination on top
	// of the position we're about to restore.
	if (data.contains("players")) {
		for (auto& [name, playerData] : data["players"].items()) {
			auto player = getPlayerByName(name);
			if (!player) continue;

			player->stopCurrentAction();

			if (playerData.contains("scene")) {
				std::string sceneName = playerData["scene"].get<std::string>();
				if (!sceneName.empty()) {
					auto scene = findScene(sceneName);
					if (scene) player->moveToScene(scene);
				}
			}

			if (playerData.contains("position")) {
				float x = playerData["position"]["x"].get<float>();
				float y = playerData["position"]["y"].get<float>();
				player->setPosition(Vector2<>(x, y));
			}

			if (playerData.contains("verb")) {
				std::string verbName = playerData["verb"].get<std::string>();
				if (!verbName.empty())
					player->setVerb(verbName);
			}

			if (playerData.contains("inventory"))
				applyInventory(player, playerData["inventory"].get<std::vector<std::string>>());
		}
	}

	// Current player
	if (data.contains("currentPlayerName")) {
		std::string name = data["currentPlayerName"].get<std::string>();
		auto player = getPlayerByName(name);
		if (player) currentPlayer = player;
	}

	// Current scene (players above are already relocated to their own saved
	// scenes; this just makes the active scene match currentPlayer's again)
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

void Game::applyInventory(std::shared_ptr<Player> player, const std::vector<std::string>& itemNames) {
	if (!player) return;
	auto& inventory = player->getInventory();
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