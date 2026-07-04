#pragma once

#include <list>
#include <string>
#include <map>
#include <unordered_map>
#include "Player.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"
#include "AudioManager.hpp"
#include "UI.hpp"
#include "GameState.hpp"

class Game {
	// Singleton
protected:
	Game();
	static Game* game_;

	std::list<std::shared_ptr<Player>> players;
	bool gameIsRunning = true;
	Renderer renderer;
	InputManager inputManager;
	std::vector<std::shared_ptr<Verb>> verbs;
	std::shared_ptr<Scene> currentScene = nullptr;
	SDL_Event event;
	std::shared_ptr<Player> currentPlayer;
	Event startEvent;
	Event endEvent;
	UI ui;
	AudioManager audio;
	GameState gameState;

	Event defaultClickEvent; // When the player clicks but no object interaction is found
	std::map<std::pair<std::string, std::string>, Event> combinations;

	std::shared_ptr<Verb> rightClickVerb = nullptr;
	std::shared_ptr<Verb> leftClickVerb = nullptr;
	std::shared_ptr<Verb> defaultVerb = nullptr;

	// Scene registry for save/load
	std::unordered_map<std::string, std::shared_ptr<Scene>> sceneRegistry;

	// Inventory item prototypes for save/load reconstruction
	std::unordered_map<std::string, std::shared_ptr<GameObject>> inventoryItemPrototypes;

public:
	// Standard Engine Verbs
	std::shared_ptr<Verb> walkVerb;
	std::shared_ptr<Verb> pushVerb;
	std::shared_ptr<Verb> closeVerb;
	std::shared_ptr<Verb> openVerb;
	std::shared_ptr<Verb> talkVerb;
	std::shared_ptr<Verb> giveVerb;
	std::shared_ptr<Verb> pickupVerb;
	std::shared_ptr<Verb> useVerb;
	std::shared_ptr<Verb> lookAtVerb;
	std::shared_ptr<Verb> pullVerb;
	std::shared_ptr<Verb> anyVerb;
	std::shared_ptr<Verb> selectedVerb;

	Game(Game& other) = delete;
	void operator=(const Game&) = delete;
	static Game& getInstance();
	std::shared_ptr<Player> getCurrentPlayer();
	void setCurrentPlayer(std::shared_ptr<Player> player);
	void addPlayer(std::shared_ptr<Player> player);
	void removePlayer(std::shared_ptr<Player> player);
	std::list<std::shared_ptr<Player>> getPlayers();
	std::shared_ptr<Player> getPlayerByName(std::string name);
	void startGame();
	std::shared_ptr<Scene> getScene();
	void changeScene(std::shared_ptr<Scene> newScene, float fadeDuration = 0.0f);
	void setStartEvent(Event);
	void setEndEvent(Event);
	void setDefaultClickEvent(Event);
	UI& getUI();
	std::shared_ptr<Verb> getVerb(std::string verbName);
	void addVerb(std::shared_ptr<Verb> verb);
	void setCustomCursor(std::shared_ptr<Texture> customCursorTexture, Vector2<> hotspot);
	void bindRightClickToVerb(std::shared_ptr<Verb> verb);
	void bindRightClickToVerb(std::string verbName);
	void bindLeftClickToVerb(std::shared_ptr<Verb> verb);
	void bindLeftClickToVerb(std::string verbName);
	std::shared_ptr<Verb> getLeftClickVerb();
	std::shared_ptr<Verb> getRightClickVerb();
	std::shared_ptr<Verb> getDefaultVerb();
	void setDefaultVerb(std::shared_ptr<Verb> verb);
	void setDefaultVerb(std::string verbName);
	void changeToDefaultVerb();
	Renderer& getRenderer();

	AudioManager& getAudio();
	GameState& getGameState();

	// Localization: switch the active language (e.g. "en", "es"). User-facing
	// text resolved through tr() picks up the change; see Localization.hpp.
	void setLocale(const std::string& locale);
	const std::string& getLocale() const;
	void addCombination(const std::string& itemName, const std::string& targetName, Event event);
	Event* findCombination(const std::string& itemName, const std::string& targetName);

	// Scene registry
	void registerScene(const std::string& name, std::shared_ptr<Scene> scene);
	std::shared_ptr<Scene> findScene(const std::string& name);
	void clearSceneRegistry();

	// Inventory prototype registry
	void registerInventoryItemPrototype(const std::string& name, std::shared_ptr<GameObject> prototype);
	std::shared_ptr<GameObject> findInventoryItemPrototype(const std::string& name);

	// Save / Load
	bool saveGame(int slotIndex);
	bool loadGame(int slotIndex);
	static std::string getSavePath(int slotIndex);

	// Quick load after scene objects are created
	void applyObjectStates(const nlohmann::json& scenesJson);
	void applyInventory(const std::vector<std::string>& itemNames);

	// Quit
	void quitGame();
};