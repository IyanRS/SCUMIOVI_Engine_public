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

	std::list<Player*> players;
	bool gameIsRunning = true;
	Renderer renderer;
	InputManager inputManager;
	Scene* currentScene;
	SDL_Event event;
	Player* currentPlayer;
	Event startEvent;
	Event endEvent;
	std::list<Verb*> verbs;
	UI ui;

	Texture* customCursorTexture;
	Vector2<> customCursorHotspot;

	Event defaultClickEvent; // When the player clicks but no object interaction is found

	Verb* rightClickVerb;
	Verb* leftClickVerb;
	Verb* defaultVerb;

public:
	Game(Game& other) = delete;
	void operator=(const Game&) = delete;
	static Game& getInstance();
	Player& getCurrentPlayer();
	void setCurrentPlayer(Player& player);
	void addPlayer(Player& player);
	void removePlayer(Player& player);
	std::list<Player*> getPlayers();
	Player* getPlayerByName(std::string name);
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
	void setDefaultVerb(Verb* verb);
	void setDefaultVerb(std::string verbName);
	void changeToDefaultVerb();
};