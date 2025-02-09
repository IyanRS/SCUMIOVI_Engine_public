#pragma once

#include "Texture.hpp"

#include <vector>
#include "GameObject.hpp"
#include "UIVerbButton.hpp"
#include <string>

#include "UIDialogueOption.hpp"

#include "DialogueNode.hpp"

class UILayout {
	std::vector<GameObject*> uiElements; //TODO: Create Wrapper class for GameObject "UIElement"
	std::vector<UIVerbButton*> uiVerbButtons;
	bool visible = true;
	std::string name;

	std::vector<UIDialogueOption*> dialogueOptions;

public:
	UILayout(std::string name);
	std::vector<GameObject*>& getUIElements();

	// TODO: add more parameters for customization: spacing, text size, text color, etc.
	//GameObject& createUIVerbButton(Verb& verb, Vector2<> position, Vector2<> size, Texture* verbBackground, Event& onUIUpdate);
	std::vector<UIVerbButton*> createVerbsMatrix(Vector2<> position, std::vector<std::vector<Verb>> verbs, Texture* verbBackground);
	GameObject& createUIItemSlot(Vector2<> position, Vector2<> size, Texture* background, Event& onUIUpdate);
	void createDialogueOptions(DialogueNode& dialogueNode);
	void updateUI();
	void createInventoryMatrix(Vector2<> position, int columns, int rows, Texture* background, Event& onUIUpdate);
	void highlightVerb(Verb& verb);
	void removeHightlightVerb();
	void addUIElement(GameObject& uiElement);
	void toggleVisibility();
	GameObject* getUIElement(std::string name);
	std::string getName();
	void clearDialogueOptions();
};