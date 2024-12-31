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
	Game::players.push_back(&player);
}

void Game::removePlayer(Player& player) {
	Game::players.remove(&player);
}

std::list<Player*> Game::getPlayers() {
	return Game::players;
}

Player* Game::getPlayerByName(std::string name) {
	for (Player* p : Game::players) {
		if (p->getName() == name) {
			return p;
		}
	}
	return nullptr;
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
				// TODO: maybe move this click handling to inputManager
				SDL_MouseButtonFlags mouseButtonFlags = SDL_GetMouseState(nullptr, nullptr);
				switch (mouseButtonFlags) {
				case SDL_BUTTON_LEFT:
					if (leftClickVerb == nullptr)
						inputManager.click(ui, *currentScene, currentPlayer->getVerb());
					else
						inputManager.click(ui, *currentScene, leftClickVerb);
					//defaultClickEvent.execute(inputManager.getClickPosition(currentScene->getCamera()));
					break;
				case 4: // TODO: should be SDL_BUTTON_RIGHT (3) SDL bug?
					if (rightClickVerb == nullptr)
						inputManager.click(ui, *currentScene, currentPlayer->getVerb());
					else
						inputManager.click(ui, *currentScene, rightClickVerb);
					//if (!inputManager.click(ui, *currentScene)) {
					//	defaultClickEvent.execute(inputManager.getClickPosition(currentScene->getCamera()));
					//}
					break;
				}
				//float xMouse;
				//float yMouse;
				//SDL_GetMouseState(&xMouse, &yMouse);
				//SDL_Log("Click detected at x=%f, y=%f", xMouse, yMouse);
				break;
			}
		}

		// Main loop
		inputManager.mouseOver(ui, *currentScene);
		renderer.render(*currentScene, ui);
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

UI& Game::getUI() {
	return ui;
}

Verb* Game::getVerb(std::string verbName) {
	for (Verb* v : verbs) {
		if (v->getName() == verbName) {
			return v;
		}
	}
	return nullptr;
}

void Game::addVerb(Verb* verb) {
	verbs.push_back(verb);
}

void Game::setCustomCursor(Texture& customCursorTexture, Vector2<> hotspot) {
	renderer.setCustomMouse(customCursorTexture, hotspot);
}

// nullptr binds the click to the selected verb
void Game::bindRightClickToVerb(Verb* verb) {
	rightClickVerb = verb;
}

void Game::bindRightClickToVerb(std::string verbName) {
	rightClickVerb = getVerb(verbName);
}

void Game::bindLeftClickToVerb(Verb* verb) {
	leftClickVerb = verb;
}

void Game::bindLeftClickToVerb(std::string verbName) {
	leftClickVerb = getVerb(verbName);
}

void Game::setDefaultVerb(Verb* verb) {
	defaultVerb = verb;
}

void Game::setDefaultVerb(std::string verbName) {
	defaultVerb = getVerb(verbName);
}

void Game::changeToDefaultVerb() {
	if (defaultVerb != nullptr) {
		currentPlayer->setVerb(defaultVerb);
	}
}