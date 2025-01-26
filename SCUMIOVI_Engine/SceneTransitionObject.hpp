#pragma once

#include "GameObject.hpp"
#include "Scene.hpp"

class SceneTransitionObject : public GameObject {
private:
	Scene scene;
public:
	SceneTransitionObject(
		std::string name,
		Vector2<> position,
		Vector2<> size,
		Scene& scene
	);
};