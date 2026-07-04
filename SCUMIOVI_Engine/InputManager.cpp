#include "InputManager.hpp"
#include <SDL3/SDL.h>
#include "Common.hpp"
#include <thread>
#include <memory>
#include "Game.hpp"

Vector2<> InputManager::getClickPosition(Camera& camera) {
	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	xMouse /= WINDOW_WIDTH / camera.getSize().x;
	yMouse /= WINDOW_HEIGHT / camera.getSize().y;

	xMouse += camera.getPosition().x;
	yMouse += camera.getPosition().y;

	return Vector2<>(xMouse, yMouse);
}

bool InputManager::checkObjectClicked(float xMouse, float yMouse, std::shared_ptr<GameObject> object, std::shared_ptr<Verb> verb) {
	if (object == nullptr) {
		SDL_Log("Invalid gameObject.");
		return false;
	}

	if (xMouse >= object->getPosition().x && xMouse < object->getPosition().x + object->getSize().x &&
		yMouse >= object->getPosition().y && yMouse < object->getPosition().y + object->getSize().y) {
		return object->onClick(Vector2<>(xMouse, yMouse), verb);
	}
	return false;
}

void InputManager::checkMouseOverObject(float xMouse, float yMouse, std::shared_ptr<GameObject> object) {
	if (object == nullptr) {
		SDL_Log("Invalid gameObject.");
		return;
	}

	if (xMouse >= object->getPosition().x && xMouse < object->getPosition().x + object->getSize().x &&
		yMouse >= object->getPosition().y && yMouse < object->getPosition().y + object->getSize().y) {
		if (!currentlyMouseOver.contains(object)) {
			currentlyMouseOver.insert(object);
			object->onMouseOver(Vector2<>(xMouse, yMouse));
		}
	}
	else {
		if (currentlyMouseOver.contains(object)) {
			currentlyMouseOver.erase(object);
			object->onMouseNotOver(Vector2<>(xMouse, yMouse));
		}
	}
}

void InputManager::mouseOver(UI& ui, Scene& scene) {
	if (ui.getLayout() == nullptr)
		return;

	auto now = std::chrono::system_clock::now();
	auto lastCheckTimeDiff = now - lastCheckTime;
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(lastCheckTimeDiff).count();

	long long msUntilNextFrame = (1000 / FPS) - ms;

	if (msUntilNextFrame > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(msUntilNextFrame));
	}

	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	xMouse /= WINDOW_WIDTH / scene.getCamera().getSize().x;
	yMouse /= WINDOW_HEIGHT / scene.getCamera().getSize().y;

	auto uiElements = ui.getUIElements();
	for (int i = (int)uiElements.size() - 1; i >= 0; i--) {
		checkMouseOverObject(xMouse, yMouse, uiElements[i]);
	}

	Camera& camera = scene.getCamera();
	xMouse += camera.getPosition().x;
	yMouse += camera.getPosition().y;

	auto gameObjects = scene.getGameObjects();
	for (int i = (int)gameObjects.size() - 1; i >= 0; i--) {
		checkMouseOverObject(xMouse, yMouse, gameObjects[i]);
	}

	lastCheckTime = now;
}

bool InputManager::click(UI& ui, Scene& scene, std::shared_ptr<Verb> verb) {
	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	if (ui.getLayout() != nullptr) {
		xMouse /= WINDOW_WIDTH / scene.getCamera().getSize().x;
		yMouse /= WINDOW_HEIGHT / scene.getCamera().getSize().y;

		auto uiElements = ui.getUIElements();
		for (int i = (int)uiElements.size() - 1; i >= 0; i--) {
			if (checkObjectClicked(xMouse, yMouse, uiElements[i], verb)) {
				return true;
			}
		}
	}

	Camera& camera = scene.getCamera();
	xMouse += camera.getPosition().x;
	yMouse += camera.getPosition().y;

	auto gameObjects = scene.getGameObjects();
	for (int i = (int)gameObjects.size() - 1; i >= 0; i--) {
		if (checkObjectClicked(xMouse, yMouse, gameObjects[i], verb)) {
			return true;
		}
	}
	return false;
}

void InputManager::handleInput(SDL_Event& event, UI& ui, Scene& scene) {
	Game& game = Game::getInstance();
	auto currentPlayer = game.getCurrentPlayer();
	if (!currentPlayer) return;

	switch (event.type) {
	case SDL_EVENT_KEY_DOWN:
		switch (event.key.key) {
		case SDLK_F5:
			if (game.saveGame(0)) {
				SDL_Log("Game saved to slot 0.");
			} else {
				SDL_Log("Failed to save game.");
			}
			break;
		case SDLK_F9:
			if (game.loadGame(0)) {
				SDL_Log("Game loaded from slot 0.");
			} else {
				SDL_Log("Failed to load game.");
			}
			break;
		}
		break;
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		SDL_MouseButtonFlags mouseButtonFlags = SDL_GetMouseState(nullptr, nullptr);
		switch (mouseButtonFlags) {
		case 0:
		case SDL_BUTTON_LEFT:
			if (game.getLeftClickVerb() == nullptr)
				click(ui, scene, currentPlayer->getVerb());
			else
				click(ui, scene, game.getLeftClickVerb());
			break;
		case 4: // SDL_BUTTON_RIGHT bitmask
			if (game.getRightClickVerb() == nullptr)
				click(ui, scene, currentPlayer->getVerb());
			else
				click(ui, scene, game.getRightClickVerb());
			break;
		}
		break;
	}
}

void InputManager::clearClicksPool() {
	// Currently a stub as part of the refactoring
}