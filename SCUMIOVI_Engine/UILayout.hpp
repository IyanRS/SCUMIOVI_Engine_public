#pragma once

#include "Texture.hpp"

#include <vector>
#include "GameObject.hpp"
#include "UIVerbButton.hpp"
#include <string>

#include "UIDialogueOption.hpp"

#include "DialogueNode.hpp"
#include "UIItemSlot.hpp"
#include "UIElement.hpp"
#include "UIVerbGrid.hpp"
#include "UIInventoryGrid.hpp"

class UILayout {
	std::vector<std::shared_ptr<GameObject>> uiElements;
	bool visible = true;
	std::string name;

	std::vector<std::shared_ptr<UIDialogueOption>> dialogueOptions;

	// Verb currently highlighted (e.g. hovering an object with a default verb);
	// applied to the matching button by whichever UIVerbGrid is present.
	std::shared_ptr<Verb> highlightedVerb;

	// Set by createInventoryMatrix; used to clamp the inventory scroll index to
	// the number of slots actually available.
	std::shared_ptr<UIInventoryGrid> inventoryGrid;
	int currentInventoryIndex = 0;

public:
	UILayout(std::string name);
	std::vector<std::shared_ptr<GameObject>>& getUIElements();
	// Flattens the element tree into a draw-ordered list with absolute positions
	// resolved (parents before their children). The renderer and input hit-test
	// consume this so nested UIElement trees work without special-casing.
	std::vector<std::shared_ptr<GameObject>> getRenderElements();

	std::vector<std::shared_ptr<UIVerbButton>> createVerbsMatrix(Vector2<> position, std::vector<std::vector<std::shared_ptr<Verb>>> verbs, std::shared_ptr<Texture> verbBackground);
	void createDialogueOptions(DialogueNode& dialogueNode);
	void updateUI();
	void createInventoryMatrix(Vector2<> position, int columns, int rows, std::shared_ptr<Texture> background, Event& onUIUpdate);
	void addUIElement(std::shared_ptr<GameObject> uiElement);
	void removeUIElement(std::shared_ptr<GameObject> uiElement);
	void toggleVisibility();
	void setHighlightedVerb(std::shared_ptr<Verb> verb);
	std::shared_ptr<Verb> getHighlightedVerb();
	std::shared_ptr<GameObject> getUIElement(std::string name);
	std::string getName();
	void clearDialogueOptions();
	void incrementInventoryIndex(int increment);
	int getInventoryIndex();
};
