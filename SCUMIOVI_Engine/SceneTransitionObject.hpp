#pragma once

#include "GameObject.hpp"
#include "Scene.hpp"

class SceneTransitionObject : public GameObject {
private:
	std::shared_ptr<Scene> targetScene;
public:
	SceneTransitionObject(
		std::string name,
		Vector2<> position,
		Vector2<> size,
		std::shared_ptr<Scene> scene
	);
	std::shared_ptr<Scene> getTargetScene();
};