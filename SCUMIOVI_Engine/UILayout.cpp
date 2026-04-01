#include "UILayout.hpp"
#include "Game.hpp"

#include "DialogueOption.hpp"

std::vector<std::shared_ptr<GameObject>>& UILayout::getUIElements() {
	return uiElements;
}

void UILayout::updateUI() {
	Game& game = Game::getInstance();
	Verb* currentVerb = game.getCurrentPlayer()->getVerb();
	if (currentVerb != nullptr) {
		// TODO: uiCurrentVerbInfo reference in UI class
		auto infoElement = getUIElement("uiCurrentVerbInfo");
		if (infoElement)
			infoElement->setTextOver(currentVerb->getInfo( game.getCurrentPlayer()->getSelectedObject().get(), game.getCurrentPlayer()->getTargetObject().get() ));
	}
	removeHightlightVerb();
	if (highlightedVerb != nullptr)
		highlightVerb(*highlightedVerb);
	//auto currentItemIndex = currentInventoryIndex;
	//for (auto inventorySlot : itemSlots) {

	//	currentItemIndex++;
	//}

	auto& elements = getUIElements();
	auto it = elements.begin();

	while (it != elements.end()) {
		if ((*it)->hasTag("item")) {
			it = elements.erase(it);
		}
		else ++it;
	}

	// TODO: avoid hardcoded values
	int currentIndex = 0;
	int columns = 4;
	int width = 40;
	int height = 22;

	for (auto& item : game.getCurrentPlayer()->getInventory().getItems()) {
		int row = currentIndex / 4;
		int column = currentIndex % 4;

		if (row < 2) {
			if (itemSlots.size() <= currentIndex)
				break;
			Vector2<> centerOffset = itemSlots[currentIndex]->getSize()/2-item->getSize()/2;

			item->setPosition(Vector2<>(150 + (column * width), 154 + (row * height)) + centerOffset);
			removeUIElement(item);
			addUIElement(item);
		}
		else {
			// Arrows logic
		}
		currentIndex++;
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
	currentInventoryIndex += increment;
}

int UILayout::getInventoryIndex() {
	return currentInventoryIndex;
}

void UILayout::createDialogueOptions(DialogueNode& dialogueNode) {
	clearDialogueOptions();
	int currentY = 152;
	for (DialogueOption& dialogueOption : dialogueNode.getOptions()) {
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

void UILayout::highlightVerb(Verb& verb) {
	removeHightlightVerb();
	for (auto& button : uiVerbButtons) {
		if (button->getVerb() == verb) {
			button->highlight();
		}
	}
}

std::vector<std::shared_ptr<UIVerbButton>> UILayout::createVerbsMatrix(Vector2<> position, std::vector<std::vector<Verb*>> verbs, Texture* verbBackground) {
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
			auto button = std::make_shared<UIVerbButton>(*verbs[j][i], buttonPos, Vector2<>(columnLengths[i] * 7, height), verbBackground);
			addUIElement(button);
			uiVerbButtons.push_back(button);
			buttons.push_back(button);
		}
	}
	return buttons;
}

std::shared_ptr<GameObject> UILayout::createUIItemSlot(int index, Vector2<> position, Vector2<> size, Texture* background, Event& onUIUpdate) {
	auto itemSlot = std::make_shared<UIItemSlot>(position, size, background, index);

	addUIElement(itemSlot);
	itemSlots.push_back(itemSlot);

	return itemSlot;
}

void UILayout::createInventoryMatrix(Vector2<> position, int columns, int rows, Texture* background, Event& onUIUpdate) {
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

void UILayout::setHighlightedVerb(Verb* verb) {
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