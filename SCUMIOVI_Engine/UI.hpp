#pragma once

#include "Texture.hpp"

#include <vector>
#include "GameObject.hpp"
#include "UIVerbButton.hpp"
#include "UILayout.hpp"

class UI {
	UILayout* currentLayout;

	std::vector<UILayout*> layouts;

//	std::vector<GameObject*> uiElements; //TODO: Create Wrapper class for GameObject "UIElement"
//	std::vector<UIVerbButton*> uiVerbButtons;
//	bool visible = true;
//

	void highlightVerb(Verb& verb);
	void removeHightlightVerb();

public:
	std::vector<GameObject*>& getUIElements();

	// TODO: add more parameters for customization: spacing, text size, text color, etc.
	//GameObject& createUIVerbButton(Verb& verb, Vector2<> position, Vector2<> size, Texture* verbBackground, Event& onUIUpdate);
	//std::vector<UIVerbButton*> createVerbsMatrix(Vector2<> position, std::vector<std::vector<Verb>> verbs, Texture* verbBackground);
	//GameObject& createUIItemSlot(Vector2<> position, Vector2<> size, Texture* background, Event& onUIUpdate);
	void updateUI();
	//void createInventoryMatrix(Vector2<> position, int columns, int rows, Texture* background, Event& onUIUpdate);

	void setHighlightedVerb(Verb* verb);

	//void addUIElement(GameObject& uiElement);
	void toggleVisibility();
	GameObject* getUIElement(std::string name);
	void setLayout(std::string name);
	void createLayout(std::string name);
	UILayout& getLayout(std::string name);
	UILayout* getLayout();
	void createDialogueOptions(DialogueNode& dialogueNode);
	void clearDialogueOptions();
	void incrementInventoryIndex(int increment);
	int getInventoryIndex();
};