#pragma once

#include "Scene.hpp"
#include "UI.hpp"
#include <unordered_set>
#include <chrono>

class InputManager {
	bool checkObjectClicked(float xMouse, float yMouse, GameObject* object, Verb* verb);
	void checkMouseOverObject(float xMouse, float yMouse, GameObject* object);
	std::unordered_set<GameObject*> currentlyMouseOver;
	std::chrono::system_clock::time_point lastCheckTime;
public:
	bool click(UI& ui, Scene& scene, Verb* verb);
	Vector2<> getClickPosition(Camera&);
	void mouseOver(UI& ui, Scene& scene);
};