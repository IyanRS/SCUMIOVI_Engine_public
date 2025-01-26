#pragma once

#include "GameObject.hpp"
#include "SceneTransitionObject.hpp"

class Door : public GameObject {
private:
	SceneTransitionObject sceneTransition;
public:
	static const enum DoorState {
		OPEN, CLOSED
	};

	Door(std::string name,
		Vector2<> position,
		Texture& closedTexture,
		Texture& openTexture,
		Scene& scene);
};