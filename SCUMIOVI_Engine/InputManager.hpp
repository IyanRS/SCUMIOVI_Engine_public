#pragma once

#include "Scene.hpp"
#include "UI.hpp"
#include "KeyBindings.hpp"
#include <unordered_set>
#include <memory>
#include <SDL3/SDL.h>

class InputManager {
	std::unordered_set<std::shared_ptr<GameObject>> currentlyMouseOver;
	KeyBindings keyBindings;
public:
	// Per-object hit tests used by the click/mouseOver loops below. An object
	// that is invisible AND non-interactable (a picked-up item) is absent to
	// the mouse; invisible-but-interactable ones (closed doors drawn by the
	// background art) still take clicks and hovers.
	bool checkObjectClicked(float xMouse, float yMouse, std::shared_ptr<GameObject> object, std::shared_ptr<Verb> verb);
	void checkMouseOverObject(float xMouse, float yMouse, std::shared_ptr<GameObject> object);

	bool click(UI& ui, Scene& scene, std::shared_ptr<Verb> verb);
	Vector2<> getClickPosition(Camera&);
	void handleInput(SDL_Event& event, UI& ui, Scene& scene);
	void mouseOver(UI& ui, Scene& scene);
	KeyBindings& getKeyBindings();
};