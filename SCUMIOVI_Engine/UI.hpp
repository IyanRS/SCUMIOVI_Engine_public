#pragma once

#include "Texture.hpp"

#include <vector>
#include "GameObject.hpp"

class UI {
	std::vector<GameObject> uiElements; //TODO: Create Wrapper class for GameObject "UIElement"
	bool visible = true;
public:
	std::vector<GameObject>& getUIElements();
	void addUIElement(GameObject& uiElement);
	void toggleVisibility();
	GameObject* getUIElement(std::string name);
};