#include "UILayout.hpp"
#include <algorithm>
#include "DialogueOption.hpp"
#include "UIDialogueOption.hpp"
#include "Game.hpp"
#include "UIElement.hpp"
#include "Common.hpp"

namespace {
	// Normalized (x, y) in [0,1] for each anchor point: (0,0) is top-left,
	// (1,1) is bottom-right, (0.5,0.5) is center, etc.
	Vector2<> anchorFraction(UIAnchor anchor) {
		switch (anchor) {
			case UIAnchor::TopLeft:      return Vector2<>(0.0f, 0.0f);
			case UIAnchor::TopCenter:    return Vector2<>(0.5f, 0.0f);
			case UIAnchor::TopRight:     return Vector2<>(1.0f, 0.0f);
			case UIAnchor::CenterLeft:   return Vector2<>(0.0f, 0.5f);
			case UIAnchor::Center:       return Vector2<>(0.5f, 0.5f);
			case UIAnchor::CenterRight:  return Vector2<>(1.0f, 0.5f);
			case UIAnchor::BottomLeft:   return Vector2<>(0.0f, 1.0f);
			case UIAnchor::BottomCenter: return Vector2<>(0.5f, 1.0f);
			case UIAnchor::BottomRight:  return Vector2<>(1.0f, 1.0f);
		}
		return Vector2<>(0.0f, 0.0f);
	}

	// Recursively appends an element and its descendants to the draw list,
	// writing each UIElement's absolute position. An element's anchor picks a
	// point within its parent's bounds (the screen, for top-level elements);
	// its offset is then applied as a nudge from that point, and the element's
	// own size is used to keep it fully inside the anchor edge it's pinned to.
	// With the default TopLeft anchor this reduces to parentAbsolute + offset,
	// so pre-existing, non-anchored layouts are unaffected.
	void flattenElement(const std::shared_ptr<GameObject>& element, Vector2<> parentAbsolute, Vector2<> parentSize,
		std::vector<std::shared_ptr<GameObject>>& out) {
		auto uiElement = std::dynamic_pointer_cast<UIElement>(element);
		if (!uiElement) {
			// Plain GameObject element: position is already absolute.
			out.push_back(element);
			return;
		}
		Vector2<> frac = anchorFraction(uiElement->getAnchor());
		Vector2<> anchorPoint(parentSize.x * frac.x, parentSize.y * frac.y);
		Vector2<> elementSize = uiElement->getSize();
		Vector2<> originOffset(elementSize.x * frac.x, elementSize.y * frac.y);

		Vector2<> absolute = parentAbsolute + anchorPoint - originOffset + uiElement->getOffset();
		uiElement->setPosition(absolute);
		out.push_back(element);
		for (auto& child : uiElement->getChildren()) {
			flattenElement(child, absolute, uiElement->getSize(), out);
		}
	}

	// Depth-first search for a named element through the tree.
	std::shared_ptr<GameObject> findElement(const std::shared_ptr<GameObject>& element, const std::string& name) {
		if (element->getName() == name) return element;
		auto uiElement = std::dynamic_pointer_cast<UIElement>(element);
		if (uiElement) {
			for (auto& child : uiElement->getChildren()) {
				if (auto found = findElement(child, name)) return found;
			}
		}
		return nullptr;
	}
}

UILayout::UILayout(std::string name) {
	this->name = name;
}

std::vector<std::shared_ptr<GameObject>>& UILayout::getUIElements() {
	return uiElements;
}

std::vector<std::shared_ptr<GameObject>> UILayout::getRenderElements() {
	std::vector<std::shared_ptr<GameObject>> out;
	Vector2<> screenSize(RENDER_WIDTH, RENDER_HEIGHT);
	for (auto& element : uiElements) {
		flattenElement(element, Vector2<>(0, 0), screenSize, out);
	}
	return out;
}

void UILayout::updateUI() {
	// Widgets (UIInfoLabel, UIVerbGrid, UIInventoryGrid, ...) own their
	// per-frame state and refresh themselves; UILayout no longer needs to know
	// about verb highlighting, inventory windowing or magic element names.
	for (auto& element : uiElements) {
		if (auto uiElement = std::dynamic_pointer_cast<UIElement>(element)) {
			uiElement->refresh();
		}
	}
}

void UILayout::clearDialogueOptions() {
	dialogueOptions.clear();
	for (auto it = uiElements.begin(); it != uiElements.end(); ) {
		if ((*it)->getName() == "option") {
			it = uiElements.erase(it);
		} else {
			++it;
		}
	}
}

void UILayout::incrementInventoryIndex(int increment) {
	int slots = inventoryGrid ? (int)inventoryGrid->getItemSlots().size() : 0;
	int maxIndex = 0;
	Game& game = Game::getInstance();
	auto player = game.getCurrentPlayer();
	if (player) {
		auto& items = player->getInventory().getItems();
		maxIndex = std::max(0, (int)items.size() - slots);
	}
	currentInventoryIndex += increment;
	currentInventoryIndex = std::max(0, std::min(currentInventoryIndex, maxIndex));
}

int UILayout::getInventoryIndex() {
	return currentInventoryIndex;
}

void UILayout::createDialogueOptions(DialogueNode& dialogueNode) {
	clearDialogueOptions();
	int currentY = 152;
	for (std::shared_ptr<DialogueOption>& dialogueOption : dialogueNode.getOptions()) {
		if (!dialogueOption->isVisible()) continue;
		auto uiDialogueOption = std::make_shared<UIDialogueOption>(dialogueOption, Vector2<>(4.0f, (float)currentY), nullptr);
		dialogueOptions.push_back(uiDialogueOption);
		addUIElement(uiDialogueOption);
		currentY += 14;
	}
}

std::vector<std::shared_ptr<UIVerbButton>> UILayout::createVerbsMatrix(Vector2<> position, std::vector<std::vector<std::shared_ptr<Verb>>> verbs, std::shared_ptr<Texture> verbBackground) {
	int height = 14;
	std::vector<int> columnLengths;

	std::vector<std::shared_ptr<UIVerbButton>> buttons;

	int columns = (int)verbs[0].size();
	int rows = (int)verbs.size();

	for (int i = 0; i < columns; i++) {
		int maxLengthColumn = 0;
		for (int j = 0; j < rows; j++) {
			if (verbs[j][i]->getDisplayName().length() > maxLengthColumn) {
				maxLengthColumn = (int)verbs[j][i]->getDisplayName().length();
			}
		}
		columnLengths.push_back(maxLengthColumn);
	}

	auto grid = std::make_shared<UIVerbGrid>("verbGrid", position);

	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			// Positions are relative to the grid; the grid itself carries `position`.
			Vector2<> buttonPos = Vector2<>(0.0f, (float)((height + 2) * j));
			for (int k = 0; k < i; k++) {
				buttonPos.x += columnLengths[k] * 7;
				buttonPos.x += 2;
			}
			auto button = std::make_shared<UIVerbButton>(verbs[j][i], buttonPos, Vector2<>((float)(columnLengths[i] * 7), (float)height), verbBackground);
			grid->addButton(button);
			buttons.push_back(button);
		}
	}

	addUIElement(grid);
	return buttons;
}

void UILayout::createInventoryMatrix(Vector2<> position, int columns, int rows, std::shared_ptr<Texture> background, Event& onUIUpdate) {
	inventoryGrid = std::make_shared<UIInventoryGrid>("inventoryGrid", position, columns, rows, Vector2<>(40.0f, 22.0f), background);
	addUIElement(inventoryGrid);
}

void UILayout::addUIElement(std::shared_ptr<GameObject> uiElement) {
	uiElements.push_back(uiElement);
}

void UILayout::removeUIElement(std::shared_ptr<GameObject> uiElement) {
	auto position = std::find(uiElements.begin(), uiElements.end(), uiElement);
	if (position != uiElements.end())
		uiElements.erase(position);
}

void UILayout::toggleVisibility() {
	visible = !visible;
}

void UILayout::setHighlightedVerb(std::shared_ptr<Verb> verb) {
	highlightedVerb = verb;
}

std::shared_ptr<Verb> UILayout::getHighlightedVerb() {
	return highlightedVerb;
}

std::shared_ptr<GameObject> UILayout::getUIElement(std::string name) {
	for (auto& element : uiElements) {
		if (auto found = findElement(element, name)) {
			return found;
		}
	}
	return nullptr;
}

std::string UILayout::getName() {
	return name;
}
