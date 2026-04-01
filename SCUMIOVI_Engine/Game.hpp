#pragma once

#include <list>
#include <string>
#include "Player.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"
#include "UI.hpp"

class Game {
	// Singleton
protected:
	Game();
	static Game* game_;

	std::list<std::shared_ptr<Player>> players;
	bool gameIsRunning = true;
	Renderer renderer;
	InputManager inputManager;
	Scene* currentScene = nullptr;
	SDL_Event event;
	std::shared_ptr<Player> currentPlayer;
	Event startEvent;
	Event endEvent;
	std::list<Verb*> verbs;
	UI ui;

	Texture* customCursorTexture;
	Vector2<> customCursorHotspot;

	Event defaultClickEvent; // When the player clicks but no object interaction is found

	Verb* rightClickVerb = nullptr;
	Verb* leftClickVerb = nullptr;
	Verb* defaultVerb = nullptr;

public:
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
	Scene& getScene();
	void changeScene(Scene& newScene);
	void setStartEvent(Event);
	void setEndEvent(Event);
	void setDefaultClickEvent(Event);
	UI& getUI();
	Verb* getVerb(std::string verbName);
	void addVerb(Verb* verb);
	void setCustomCursor(Texture& customCursorTexture, Vector2<> hotspot);
	void bindRightClickToVerb(Verb* verb);
	void bindRightClickToVerb(std::string verbName);
	void bindLeftClickToVerb(Verb* verb);
	void bindLeftClickToVerb(std::string verbName);
	Verb* getDefaultVerb();
	void setDefaultVerb(Verb* verb);
	void setDefaultVerb(std::string verbName);
	void changeToDefaultVerb();
	void clearClicksPool();
};