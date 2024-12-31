#include "UI.hpp"

std::vector<GameObject> UI::getUIElements() {
	return uiElements;
}

void UI::addUIElement(GameObject& uiElement) {
	uiElements.push_back(uiElement);
}

void UI::toggleVisibility() {
	visible = !visible;
}

GameObject* UI::getUIElement(std::string name) {
	for (GameObject& element : uiElements) {
		if (element.getName() == name) {
			return &element;
		}
	}
	return nullptr;
}