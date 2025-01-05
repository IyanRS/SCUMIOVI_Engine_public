#include "InputManager.hpp"

#include <SDL3/SDL.h>

#include "Common.hpp"

Vector2<> InputManager::getClickPosition(Camera& camera) {
	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	xMouse /= WINDOW_WIDTH / RENDER_WIDTH;
	yMouse /= WINDOW_HEIGHT / RENDER_HEIGHT;

	xMouse += camera.getPosition().x;
	yMouse += camera.getPosition().y;

	return Vector2<>(xMouse, yMouse);
}

bool InputManager::checkObjectClicked(float xMouse, float yMouse, GameObject* object, Verb* verb) {
	if (object == nullptr) {
		SDL_Log("Invalid gameObject.");
	}

	if (xMouse >= object->getPosition().x && xMouse < object->getPosition().x + object->getSize().x &&
		yMouse >= object->getPosition().y && yMouse < object->getPosition().y + object->getSize().y) {
		//SDL_Log("Object clicked");
		return object->onClick(Vector2<>(xMouse, yMouse), verb);
	}
	return false;
}

void InputManager::checkMouseOverObject(float xMouse, float yMouse, GameObject* object) {
	if (object == nullptr) {
		SDL_Log("Invalid gameObject.");
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
	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	// Transform click screen position to game position
	xMouse /= WINDOW_WIDTH / RENDER_WIDTH;
	yMouse /= WINDOW_HEIGHT / RENDER_HEIGHT;

	std::vector uiElements = ui.getUIElements();
	for (int i = uiElements.size() - 1; i >= 0; i--) {
		checkMouseOverObject(xMouse, yMouse, &uiElements[i]);
	}

	Camera& camera = scene.getCamera();
	xMouse += camera.getPosition().x;
	yMouse += camera.getPosition().y;

	std::vector gameObjects = scene.getGameObjects();
	for (int i = gameObjects.size() - 1; i >= 0; i--) {

		checkMouseOverObject(xMouse, yMouse, gameObjects[i]);
	}
}

bool InputManager::click(UI& ui, Scene& scene, Verb* verb) {
	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	// Transform click screen position to game position
	xMouse /= WINDOW_WIDTH / RENDER_WIDTH;
	yMouse /= WINDOW_HEIGHT / RENDER_HEIGHT;

	std::vector uiElements = ui.getUIElements();
	for (int i = uiElements.size()-1; i >= 0; i--) {
		if (checkObjectClicked(xMouse, yMouse, &uiElements[i], verb)) {
			return true;
		}
	}

	Camera& camera = scene.getCamera();
	xMouse += camera.getPosition().x;
	yMouse += camera.getPosition().y;

	std::vector gameObjects = scene.getGameObjects();
	for (int i = gameObjects.size() - 1; i >= 0; i--) {
		if (checkObjectClicked(xMouse, yMouse, gameObjects[i], verb)) {
			return true;
		}
	}
	return false;
	//if (scene.getWalkableArea() != nullptr) {
	//	if (scene.getWalkableArea()->isPointInside(Vector2<>(xMouse, yMouse))) {
	//		scene.getWalkableArea()->onClick(Vector2<>(xMouse, yMouse));
	//	}
	//}
}