#include "Game.hpp"

#include <SDL3/SDL.h>

Game::Game() {}

Game* Game::game_ = nullptr;

Player& Game::getCurrentPlayer() {
	return *currentPlayer;
}

void Game::setCurrentPlayer(Player& player) {
	currentPlayer = &player;
}

Game& Game::getInstance() {
	if (game_ == nullptr)
		game_ = new Game();
	return *game_;
}

void Game::addPlayer(Player& player) {
	Game::players.push_back(player);
}

void Game::removePlayer(Player& player) {
	Game::players.remove(player);
}

std::list<Player> Game::getPlayers() {
	return Game::players;
}

Player Game::getPlayerByName(std::string name) {
	for (Player p : Game::players) {
		if (p.getName() == name) {
			return p;
		}
	}
}

Scene& Game::getScene() {
	return *currentScene;
}

void Game::changeScene(Scene& newScene) {
	currentScene = &newScene;
}

void Game::startGame() {
	if (currentScene == nullptr) {
		SDL_Log("An initial scene is required.");
		return;
	}
	startEvent.execute();
	while (gameIsRunning) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				gameIsRunning = false;
				SDL_Log("SDL3 event quit");
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				if (!inputManager.click(*currentScene)) {
					defaultClickEvent.execute(inputManager.getClickPosition(currentScene->getCamera()));
				}
				//float xMouse;
				//float yMouse;
				//SDL_GetMouseState(&xMouse, &yMouse);
				//SDL_Log("Click detected at x=%f, y=%f", xMouse, yMouse);
				break;
			}
		}

		// Main loop
		renderer.render(*Game::currentScene);
	}
	endEvent.execute();
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