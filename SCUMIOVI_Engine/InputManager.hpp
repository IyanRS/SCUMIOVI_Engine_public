#pragma once

#include "Scene.hpp"
#include "UI.hpp"

class InputManager {
	bool checkObjectClicked(float xMouse, float yMouse, GameObject* object, Verb* verb);
public:
	bool click(UI& ui, Scene& scene, Verb* verb);
	Vector2<> getClickPosition(Camera&);
};