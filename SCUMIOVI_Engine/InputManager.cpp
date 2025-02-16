#include "InputManager.hpp"

#include <SDL3/SDL.h>

#include "Common.hpp"
#include <thread>

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

	// Transform click screen position to game position
	xMouse /= WINDOW_WIDTH / scene.getCamera().getSize().x;
	yMouse /= WINDOW_HEIGHT / scene.getCamera().getSize().y;

	std::vector<GameObject*> uiElements = ui.getUIElements();
	for (int i = uiElements.size() - 1; i >= 0; i--) {
		checkMouseOverObject(xMouse, yMouse, uiElements[i]);
	}

	Camera& camera = scene.getCamera();
	xMouse += camera.getPosition().x;
	yMouse += camera.getPosition().y;

	std::vector<GameObject*> gameObjects = scene.getGameObjects();
	for (int i = gameObjects.size() - 1; i >= 0; i--) {

		checkMouseOverObject(xMouse, yMouse, gameObjects[i]);
	}

	lastCheckTime = now;
}

bool InputManager::click(UI& ui, Scene& scene, Verb* verb) {
	//SDL_Log("Click");

	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	// Transform click screen position to game position
	xMouse /= WINDOW_WIDTH / scene.getCamera().getSize().x;
	yMouse /= WINDOW_HEIGHT / scene.getCamera().getSize().y;

	std::vector uiElements = ui.getUIElements();
	for (int i = uiElements.size()-1; i >= 0; i--) {
		if (checkObjectClicked(xMouse, yMouse, uiElements[i], verb)) {
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
}