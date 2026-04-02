#pragma once

#include "Texture.hpp"

#include <vector>
#include "GameObject.hpp"
#include "UIVerbButton.hpp"
#include <string>

#include "UIDialogueOption.hpp"

#include "DialogueNode.hpp"
#include "UIItemSlot.hpp"

class UILayout {
	std::vector<std::shared_ptr<GameObject>> uiElements; //TODO: Create Wrapper class for GameObject "UIElement"
	std::vector<std::shared_ptr<UIVerbButton>> uiVerbButtons;
	bool visible = true;
	std::string name;

	std::vector<std::shared_ptr<UIDialogueOption>> dialogueOptions;

	std::shared_ptr<Verb> highlightedVerb;

	std::vector<std::shared_ptr<UIItemSlot>> itemSlots;
	int currentInventoryIndex = 0;

public:
	UILayout(std::string name);
	std::vector<std::shared_ptr<GameObject>>& getUIElements();

	// TODO: add more parameters for customization: spacing, text size, text color, etc.
	//GameObject& createUIVerbButton(Verb& verb, Vector2<> position, Vector2<> size, Texture* verbBackground, Event& onUIUpdate);
	std::vector<std::shared_ptr<UIVerbButton>> createVerbsMatrix(Vector2<> position, std::vector<std::vector<std::shared_ptr<Verb>>> verbs, std::shared_ptr<Texture> verbBackground);
	std::shared_ptr<GameObject> createUIItemSlot(int index, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> background, Event& onUIUpdate);
	void createDialogueOptions(DialogueNode& dialogueNode);
	void updateUI();
	void createInventoryMatrix(Vector2<> position, int columns, int rows, std::shared_ptr<Texture> background, Event& onUIUpdate);
	void highlightVerb(std::shared_ptr<Verb> verb);
	void removeHightlightVerb();
	void addUIElement(std::shared_ptr<GameObject> uiElement);
	void removeUIElement(std::shared_ptr<GameObject> uiElement);
	void toggleVisibility();
	void setHighlightedVerb(std::shared_ptr<Verb> verb);
	std::shared_ptr<GameObject> getUIElement(std::string name);
	std::string getName();
	void clearDialogueOptions();
	void incrementInventoryIndex(int increment);
	int getInventoryIndex();
};