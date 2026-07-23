#include "InputManager.hpp"
#include <SDL3/SDL.h>
#include "Common.hpp"
#include <memory>
#include "Game.hpp"
#include "Cutscene.hpp"

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

	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	xMouse /= WINDOW_WIDTH / scene.getCamera().getSize().x;
	yMouse /= WINDOW_HEIGHT / scene.getCamera().getSize().y;

	auto uiElements = ui.getRenderElements();
	for (int i = (int)uiElements.size() - 1; i >= 0; i--) {
		checkMouseOverObject(xMouse, yMouse, uiElements[i]);
	}

	// A pausing overlay (see UI::openOverlay) keeps the scene from receiving
	// hover state while it's shown, matching the click-blocking below.
	if (ui.isGamePaused()) return;

	Camera& camera = scene.getCamera();
	xMouse += camera.getPosition().x;
	yMouse += camera.getPosition().y;

	auto gameObjects = scene.getGameObjects();
	for (int i = (int)gameObjects.size() - 1; i >= 0; i--) {
		checkMouseOverObject(xMouse, yMouse, gameObjects[i]);
	}
}

bool InputManager::click(UI& ui, Scene& scene, std::shared_ptr<Verb> verb) {
	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	if (ui.getLayout() != nullptr) {
		xMouse /= WINDOW_WIDTH / scene.getCamera().getSize().x;
		yMouse /= WINDOW_HEIGHT / scene.getCamera().getSize().y;

		auto uiElements = ui.getRenderElements();
		for (int i = (int)uiElements.size() - 1; i >= 0; i--) {
			if (checkObjectClicked(xMouse, yMouse, uiElements[i], verb)) {
				return true;
			}
		}
	}

	// While a pausing overlay is active, only UI (base layout + overlay)
	// receives clicks — the world underneath is frozen and shouldn't react.
	if (ui.isGamePaused()) return false;

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
		// Bindings are game-provided data (see assets/keybindings.json); the
		// engine has no hardcoded keys of its own.
		keyBindings.handleKeyDown(event.key.key);
		break;
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		// While a cutscene plays, the world is on rails: no walking, no verb
		// clicks, no object interactions. Keys are NOT held back — the skip
		// binding (see "skipCutscene" in the game's keybindings) must work.
		if (Cutscene::isPlaying())
			break;
		SDL_MouseButtonFlags mouseButtonFlags = SDL_GetMouseState(nullptr, nullptr);
		switch (mouseButtonFlags) {
		case 0:
		case SDL_BUTTON_LMASK:
			if (game.getLeftClickVerb() == nullptr)
				click(ui, scene, currentPlayer->getVerb());
			else
				click(ui, scene, game.getLeftClickVerb());
			break;
		case SDL_BUTTON_RMASK:
			if (game.getRightClickVerb() == nullptr)
				click(ui, scene, currentPlayer->getVerb());
			else
				click(ui, scene, game.getRightClickVerb());
			break;
		}
		break;
	}
}

KeyBindings& InputManager::getKeyBindings() {
	return keyBindings;
}