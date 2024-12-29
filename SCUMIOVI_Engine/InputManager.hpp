#pragma once

#include "Scene.hpp"
#include "UI.hpp"

class InputManager {
	bool checkObjectClicked(float xMouse, float yMouse, GameObject* object);
public:
	bool click(UI& ui, Scene& scene);
	Vector2<> getClickPosition(Camera&);
};