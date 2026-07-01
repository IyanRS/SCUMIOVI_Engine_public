#include "Game.hpp"

#include <SDL3/SDL.h>
#include <memory>
#include <algorithm>

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

	lookAtVerb->setDefaultEvent(Event([](EventInfo e) {
		e.player.say("No veo nada especial.");
	}));
	pushVerb->setDefaultEvent(Event([](EventInfo e) {
		e.player.say("No puedo mover eso.");
	}));
	pullVerb->setDefaultEvent(Event([](EventInfo e) {
		e.player.say("No puedo mover eso.");
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

void Game::changeScene(std::shared_ptr<Scene> newScene) {
	currentScene = newScene;
	if (currentPlayer) {
		currentPlayer->moveToScene(newScene);
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

void Game::clearClicksPool() {
	inputManager.clearClicksPool();
}