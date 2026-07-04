#pragma once

#include "Scene.hpp"
#include "UI.hpp"
#include <unordered_set>
#include <memory>
#include <SDL3/SDL.h>

class InputManager {
	bool checkObjectClicked(float xMouse, float yMouse, std::shared_ptr<GameObject> object, std::shared_ptr<Verb> verb);
	void checkMouseOverObject(float xMouse, float yMouse, std::shared_ptr<GameObject> object);
	std::unordered_set<std::shared_ptr<GameObject>> currentlyMouseOver;
public:
	bool click(UI& ui, Scene& scene, std::shared_ptr<Verb> verb);
	Vector2<> getClickPosition(Camera&);
	void handleInput(SDL_Event& event, UI& ui, Scene& scene);
	void mouseOver(UI& ui, Scene& scene);
};