#pragma once

#include <list>
#include <string>
#include <map>
#include <unordered_map>
#include <functional>
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

	// Named interaction scripts, referenceable by name from data-driven UI,
	// object interactions and dialogue.
	std::unordered_map<std::string, std::function<void(EventInfo)>> scripts;

	std::shared_ptr<Verb> rightClickVerb = nullptr;
	std::shared_ptr<Verb> leftClickVerb = nullptr;
	std::shared_ptr<Verb> defaultVerb = nullptr;

	// Scene registry for save/load
	std::unordered_map<std::string, std::shared_ptr<Scene>> sceneRegistry;

	// Inventory item prototypes for save/load reconstruction
	std::unordered_map<std::string, std::shared_ptr<GameObject>> inventoryItemPrototypes;
	// Per-slot save timestamps; see getSaveTimestamp().
	std::unordered_map<int, std::string> saveTimestamps;

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

	// -- Multi-character party switching --------------------------------------
	// Makes `player` the active, player-controlled character. If it's already
	// in a different scene than the one currently showing, the active scene
	// switches to that one (optionally fading, like changeScene) — mirroring
	// classic multi-character SCUMM games where party members can be off in
	// different rooms simultaneously. Each Player already tracks its own verb
	// selection, selected/target object and inventory independently, so those
	// carry over untouched. Camera follow behavior is NOT changed automatically
	// (a game using Camera::setObjectToFollow should re-point it itself if it
	// wants the camera to track whichever character is currently active).
	// Returns false if player is null or was never added via addPlayer.
	bool switchToPlayer(std::shared_ptr<Player> player, float fadeDuration = 0.0f);
	bool switchToPlayer(const std::string& name, float fadeDuration = 0.0f);
	// Cycles to the next/previous player in the order they were added via
	// addPlayer, wrapping around. Returns false with fewer than two players,
	// or if the current player isn't part of the party.
	bool switchToNextPlayer(float fadeDuration = 0.0f);
	bool switchToPreviousPlayer(float fadeDuration = 0.0f);

	void startGame();
	std::shared_ptr<Scene> getScene();
	// callback fires once the transition is fully done — after the
	// fade-from-black too, if fadeDuration > 0 — not just after the swap.
	void changeScene(std::shared_ptr<Scene> newScene, float fadeDuration = 0.0f, Event callback = Event());
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
	InputManager& getInputManager();

	AudioManager& getAudio();
	GameState& getGameState();

	// Localization: switch the active language (e.g. "en", "es"). User-facing
	// text resolved through tr() picks up the change; see Localization.hpp.
	void setLocale(const std::string& locale);
	const std::string& getLocale() const;
	void addCombination(const std::string& itemName, const std::string& targetName, Event event);
	Event* findCombination(const std::string& itemName, const std::string& targetName);

	// -- Named script registry -----------------------------------------------
	// Register a developer script under a name so data (UI actions, object and
	// dialogue interactions) can invoke it. Two forms are accepted: one that
	// receives the triggering EventInfo and one that ignores it.
	void registerScript(const std::string& name, std::function<void(EventInfo)> script);
	void registerScript(const std::string& name, std::function<void()> script);
	bool hasScript(const std::string& name) const;
	// Returns an Event that looks up and runs the named script when executed
	// (late-bound, so registration order does not matter).
	Event scriptEvent(const std::string& name);

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
	// Human-readable "when was this slot saved" (e.g. "2026-07-23 14:33");
	// empty if the slot has no save. Cached — reads the file at most once
	// per slot, and saving refreshes the cache.
	std::string getSaveTimestamp(int slotIndex);

	// Quick load after scene objects are created
	void applyObjectStates(const nlohmann::json& scenesJson);
	void applyInventory(std::shared_ptr<Player> player, const std::vector<std::string>& itemNames);

	// Quit
	void quitGame();
};