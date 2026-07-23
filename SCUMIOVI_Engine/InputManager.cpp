#include "InputManager.hpp"
#include <SDL3/SDL.h>
#include "Common.hpp"
#include <memory>
#include "Game.hpp"
#include "Cutscene.hpp"
#include "UILayout.hpp"

Vector2<> InputManager::getClickPosition(Camera& camera) {
	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	Vector2<> windowSize = Game::getInstance().getRenderer().getWindowSize();
	xMouse /= windowSize.x / camera.getSize().x;
	yMouse /= windowSize.y / camera.getSize().y;

	xMouse += camera.getPosition().x;
	yMouse += camera.getPosition().y;

	return Vector2<>(xMouse, yMouse);
}

bool InputManager::checkObjectClicked(float xMouse, float yMouse, std::shared_ptr<GameObject> object, std::shared_ptr<Verb> verb) {
	if (object == nullptr) {
		SDL_Log("Invalid gameObject.");
		return false;
	}

	// "Invisible AND non-interactable" means absent (a picked-up item):
	// the mouse ignores it. Invisible but still interactable means the
	// object is merely not drawn — like a closed door whose art is part of
	// the background — and it must keep taking clicks and hovers.
	if (!object->isVisible() && !object->isInteractable()) return false;

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

	// Absent objects (invisible AND non-interactable — picked-up items)
	// can't be hovered; if one goes absent while hovered (picked up under
	// the cursor), the else branch runs its mouse-out so hover state doesn't
	// stick. Invisible-but-interactable objects (closed doors drawn by the
	// background) hover normally.
	if ((object->isVisible() || object->isInteractable()) &&
		xMouse >= object->getPosition().x && xMouse < object->getPosition().x + object->getSize().x &&
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

	Vector2<> windowSize = Game::getInstance().getRenderer().getWindowSize();
	xMouse /= windowSize.x / scene.getCamera().getSize().x;
	yMouse /= windowSize.y / scene.getCamera().getSize().y;

	// Mirror the click gating: during a cutscene only an overlay gets hover
	// state, so the held-back verb bar doesn't light up under the pointer.
	auto uiElements = Cutscene::isPlaying()
		? (ui.getOverlay() ? ui.getOverlay()->getRenderElements()
		                   : std::vector<std::shared_ptr<GameObject>>())
		: ui.getRenderElements();
	for (int i = (int)uiElements.size() - 1; i >= 0; i--) {
		checkMouseOverObject(xMouse, yMouse, uiElements[i]);
	}

	// A pausing overlay (see UI::openOverlay) keeps the scene from receiving
	// hover state while it's shown, matching the click-blocking below.
	if (ui.isGamePaused()) return;
	if (Cutscene::isPlaying()) return;

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
		Vector2<> windowSize = Game::getInstance().getRenderer().getWindowSize();
		xMouse /= windowSize.x / scene.getCamera().getSize().x;
		yMouse /= windowSize.y / scene.getCamera().getSize().y;

		// While a cutscene plays, the base layout (verbs, inventory) is on
		// hold along with the world — but an overlay (the Esc menu) opened
		// on top of it still works. Keys are never held back, so the skip
		// binding and the menu binding keep working during the intro.
		auto uiElements = Cutscene::isPlaying()
			? (ui.getOverlay() ? ui.getOverlay()->getRenderElements()
			                   : std::vector<std::shared_ptr<GameObject>>())
			: ui.getRenderElements();
		for (int i = (int)uiElements.size() - 1; i >= 0; i--) {
			if (checkObjectClicked(xMouse, yMouse, uiElements[i], verb)) {
				return true;
			}
		}
	}

	// While a pausing overlay is active, only UI (base layout + overlay)
	// receives clicks — the world underneath is frozen and shouldn't react.
	if (ui.isGamePaused()) return false;

	// The world is on rails during a cutscene: no walking, no object clicks.
	if (Cutscene::isPlaying()) return false;

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