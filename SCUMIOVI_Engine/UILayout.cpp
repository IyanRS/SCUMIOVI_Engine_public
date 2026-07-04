#include "UILayout.hpp"
#include <algorithm>
#include "DialogueOption.hpp"
#include "UIDialogueOption.hpp"
#include "Game.hpp"

#include "DialogueOption.hpp"

std::vector<std::shared_ptr<GameObject>>& UILayout::getUIElements() {
	return uiElements;
}

void UILayout::updateUI() {
	Game& game = Game::getInstance();
	std::shared_ptr<Verb> currentVerb = game.getCurrentPlayer()->getVerb();
	if (currentVerb != nullptr) {
		// TODO: uiCurrentVerbInfo reference in UI class
		auto infoElement = getUIElement("uiCurrentVerbInfo");
		if (infoElement) {
			infoElement->setTextOver(currentVerb->getInfo(game.getCurrentPlayer()->getSelectedObject(), game.getCurrentPlayer()->getTargetObject()));
		}
	}
	removeHightlightVerb();
	if (highlightedVerb != nullptr)
		highlightVerb(highlightedVerb);
	// Remove stale item UI elements from previous frame
	auto& elements = getUIElements();
	for (auto it = elements.begin(); it != elements.end(); ) {
		if ((*it)->hasTag("item")) {
			it = elements.erase(it);
		}
		else ++it;
	}

	// Render inventory items using a sliding window over the item list.
	// The window starts at currentInventoryIndex and fills the available slots.
	// TODO: avoid hardcoded values
	int columns = 4;
	int width = 40;
	int height = 22;
	int slots = (int)itemSlots.size();

	auto& items = game.getCurrentPlayer()->getInventory().getItems();

	for (int i = 0; i < slots; i++) {
		int itemIndex = currentInventoryIndex + i;
		if (itemIndex >= (int)items.size()) break;

		auto& item = items[itemIndex];
		int row = i / columns;
		int column = i % columns;

		Vector2<> centerOffset = itemSlots[i]->getSize() / 2 - item->getSize() / 2;
		item->setPosition(Vector2<>(150 + (column * width), 154 + (row * height)) + centerOffset);
		removeUIElement(item);
		addUIElement(item);
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
	int slots = (int)itemSlots.size();
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
		auto uiDialogueOption = std::make_shared<UIDialogueOption>(dialogueOption, Vector2<>(4, currentY), nullptr);
		dialogueOptions.push_back(uiDialogueOption);
		addUIElement(uiDialogueOption);
		currentY += 14;
	}
}

void UILayout::removeHightlightVerb() {
	for (auto& button : uiVerbButtons) {
		button->setTextOver(button->getTextOver().toString());
	}
}

void UILayout::highlightVerb(std::shared_ptr<Verb> verb) {
	removeHightlightVerb();
	for (auto& button : uiVerbButtons) {
		if (button->getVerb() == verb) {
			button->highlight();
		}
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
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			Vector2<> buttonPos = Vector2<>(position.x, position.y + (height + 2) * j);
			for (int k = 0; k < i; k++) {
				buttonPos.x += columnLengths[k] * 7;
				buttonPos.x += 2;
			}
			auto button = std::make_shared<UIVerbButton>(verbs[j][i], buttonPos, Vector2<>(columnLengths[i] * 7, height), verbBackground);
			addUIElement(button);
			uiVerbButtons.push_back(button);
			buttons.push_back(button);
		}
	}
	return buttons;
}

std::shared_ptr<GameObject> UILayout::createUIItemSlot(int index, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> background, Event& onUIUpdate) {
	auto itemSlot = std::make_shared<UIItemSlot>(position, size, background, index);

	addUIElement(itemSlot);
	itemSlots.push_back(itemSlot);

	return itemSlot;
}

void UILayout::createInventoryMatrix(Vector2<> position, int columns, int rows, std::shared_ptr<Texture> background, Event& onUIUpdate) {
	int width = 40;
	int height = 22;

	for (int j = 0; j < rows; j++) {
		for (int i = 0; i < columns; i++) {
			Vector2<> slotPos = Vector2<>(position.x + (width + 2) * i, position.y + (height + 2) * j);
			createUIItemSlot(i + j * columns, slotPos, Vector2<>(width, height), background, onUIUpdate);
		}
	}
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

std::shared_ptr<GameObject> UILayout::getUIElement(std::string name) {
	for (auto& element : uiElements) {
		if (element->getName() == name) {
			return element;
		}
	}
	return nullptr;
}

std::string UILayout::getName() {
	return name;
}

UILayout::UILayout(std::string name) {
	this->name = name;
}

//void UILayout::createDialogueOptions(DialogueNode& dialogueNode) {
//	clearDialogueOptions();
//	int i = 0;
//	for (auto& option : dialogueNode.getOptions()) {
//		auto uiOption = std::make_shared<UIDialogueOption>(option, Vector2<>(10.0f, 10.0f + 20 * i), nullptr);
//		addUIElement(uiOption);
//		dialogueOptions.push_back(uiOption);
//		i++;
//	}
//}

//void UILayout::clearDialogueOptions() {
//	for (auto& option : dialogueOptions) {
//		removeUIElement(option);
//	}
//	dialogueOptions.clear();
//}
//
//void UILayout::incrementInventoryIndex(int increment) {
//	currentInventoryIndex += increment;
//}
//
//int UILayout::getInventoryIndex() {
//	return currentInventoryIndex;
//}