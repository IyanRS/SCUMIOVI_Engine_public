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

bool InputManager::click(Scene& scene) {
	float xMouse;
	float yMouse;

	SDL_GetMouseState(&xMouse, &yMouse);

	xMouse /= WINDOW_WIDTH / RENDER_WIDTH;
	yMouse /= WINDOW_HEIGHT / RENDER_HEIGHT;

	Camera& camera = scene.getCamera();
	xMouse += camera.getPosition().x;
	yMouse += camera.getPosition().y;

	SDL_Log("MouseX = %f, MouseY = %f", xMouse, yMouse);
	for (GameObject* object : scene.getGameObjects()) {
		if (object == nullptr) {
			SDL_Log("Invalid gameObject.");
		}
		// TODO: sort by layers to prioritize clicks on top layers

		if (xMouse >= object->getPosition().x && xMouse < object->getPosition().x + object->getSize().x &&
			yMouse >= object->getPosition().y && yMouse < object->getPosition().y + object->getSize().y) {
			//SDL_Log("Object clicked");
			if (object->onClick(Vector2<>(xMouse, yMouse))) {
				return true;
			}
		}
	}
	return false;
	//if (scene.getWalkableArea() != nullptr) {
	//	if (scene.getWalkableArea()->isPointInside(Vector2<>(xMouse, yMouse))) {
	//		scene.getWalkableArea()->onClick(Vector2<>(xMouse, yMouse));
	//	}
	//}
}