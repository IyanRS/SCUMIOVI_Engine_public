#pragma once

#include "GameObject.hpp"
#include "SceneTransitionObject.hpp"
#include <memory>

class Door : public GameObject {
private:
	SceneTransitionObject sceneTransition;
public:
	static const enum DoorState {
		OPEN, CLOSED
	};

	Door(std::string name,
		Vector2<> position,
		std::shared_ptr<Texture> closedTexture,
		std::shared_ptr<Texture> openTexture,
		std::shared_ptr<Scene> scene);
};