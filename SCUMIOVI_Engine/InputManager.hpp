#pragma once

#include "Scene.hpp"
#include "UI.hpp"

class InputManager {
	bool checkObjectClicked(float xMouse, float yMouse, GameObject* object, Verb* verb);
	void checkMouseOverObject(float xMouse, float yMouse, GameObject* object);
public:
	bool click(UI& ui, Scene& scene, Verb* verb);
	Vector2<> getClickPosition(Camera&);
	void mouseOver(UI& ui, Scene& scene);
};