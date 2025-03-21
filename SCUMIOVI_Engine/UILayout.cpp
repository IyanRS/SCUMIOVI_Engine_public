#include "UILayout.hpp"
#include "Game.hpp"

#include "DialogueOption.hpp"

std::vector<GameObject*>& UILayout::getUIElements() {
	return uiElements;
}

void UILayout::updateUI() {
	Game& game = Game::getInstance();
	Verb* currentVerb = game.getCurrentPlayer().getVerb();
	if (currentVerb != nullptr) {
		// TODO: uiCurrentVerbInfo reference in UI class
		if (getUIElement("uiCurrentVerbInfo"))
			getUIElement("uiCurrentVerbInfo")->setTextOver(currentVerb->getInfo( game.getCurrentPlayer().getSelectedObject(), game.getCurrentPlayer().getTargetObject() ));
			//getUIElement("uiCurrentVerbInfo")->setTextOver(currentVerb->getInfo({ game.getCurrentPlayer().getSelectedObject(), game.getCurrentPlayer().getTargetObject() }));
	}
	removeHightlightVerb();
	if (highlightedVerb != nullptr)
		highlightVerb(*highlightedVerb);
	//auto currentItemIndex = currentInventoryIndex;
	//for (auto inventorySlot : itemSlots) {

	//	currentItemIndex++;
	//}

	//std::vector< GameObject* >& uiElements = e.game.getUI().getUIElements();
	std::vector< GameObject* >::iterator it = getUIElements().begin();
	std::vector<GameObject*>& uiElements = getUIElements();

	while (it != uiElements.end()) {
		GameObject* element = (*it);
		if (element->hasTag("item")) {
			it = uiElements.erase(it);
		}
		else ++it;
	}

	// TODO: avoid hardcoded values
	int currentIndex = 0;
	int columns = 4;
	int width = 40;
	int height = 22;

	for (GameObject* item : game.getCurrentPlayer().getInventory().getItems()) {
		int row = currentIndex / 4;
		int column = currentIndex % 4;

		if (row < 2) {
			if (itemSlots.size() <= currentIndex)
				break;
			Vector2<> centerOffset = itemSlots[currentIndex]->getSize()/2-item->getSize()/2;

			item->setPosition(Vector2<>(150 + (column * width), 154 + (row * height)) + centerOffset);
			removeUIElement(*item);
			addUIElement(*item);
		}
		else {
			// Arrows logic
		}
		currentIndex++;
	}
}

void UILayout::clearDialogueOptions() {
	dialogueOptions.clear();
	std::vector<int> indexesToDelete;
	for (int i = 0; i < uiElements.size(); i++) {
		if (uiElements[i]->getName() == "option") {
			uiElements.erase(uiElements.begin() + i);
			i--;
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
		UIDialogueOption* uiDialogueOption = new UIDialogueOption(dialogueOption, Vector2<>(4, currentY), nullptr); // TODO: free memory
		dialogueOptions.push_back(uiDialogueOption);
		addUIElement(*uiDialogueOption);
		currentY += 14;
	}
}

void UILayout::removeHightlightVerb() {
	Game& game = Game::getInstance();
	for (UIVerbButton* button : uiVerbButtons) {
		button->setTextOver(button->getTextOver().toString());
		//if (uiElement.getName() == verb.getName() + "Button") {
		//	auto buttonText = uiElement.getTextOver();
		//	int highlightedColor[4] = { 230, 44, 230, 0xff };
		//	Text highlightedText = Text(buttonText.toString(), buttonText.getSize(), highlightedColor, buttonText.hasOutline(), buttonText.getTextOverMode(), buttonText.hasShadow());
		//	uiElement.setTextOver(highlightedText);
		//}
		//else {
		//	uiElement.setTextOver(uiElement.getTextOver().toString());
		//}
	}
}

void UILayout::highlightVerb(Verb& verb) {
	Game& game = Game::getInstance();
	removeHightlightVerb();
	for (UIVerbButton* button : uiVerbButtons) {
		if (button->getVerb() == verb) {
			button->highlight();
		}
	}
}

std::vector<UIVerbButton*> UILayout::createVerbsMatrix(Vector2<> position, std::vector<std::vector<Verb*>> verbs, Texture* verbBackground) {
	int height = 14;
	std::vector<int> columnLengths;

	std::vector<UIVerbButton*> buttons;

	int columns = verbs[0].size();
	int rows = verbs.size();

	for (int i = 0; i < columns; i++) {
		int maxLengthColumn = 0;
		for (int j = 0; j < rows; j++) {
			if (verbs[j][i]->getDisplayName().length() > maxLengthColumn) {
				maxLengthColumn = verbs[j][i]->getDisplayName().length();
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
			UIVerbButton* button = new UIVerbButton(*verbs[j][i], buttonPos, Vector2<>(columnLengths[i] * 7, height), verbBackground);
			addUIElement(*button);
			uiVerbButtons.push_back(button);
			buttons.push_back(button);
		}
	}
	return buttons;
}

GameObject& UILayout::createUIItemSlot(int index, Vector2<> position, Vector2<> size, Texture* background, Event& onUIUpdate) {



	//GameObject* itemSlot = new GameObject("itemSlot" + position.toString(), position, size, background);
	UIItemSlot* itemSlot = new UIItemSlot(position, size, background, index);

	//itemSlot->setDefaultVerb(&Verb::anyVerb);
	//itemSlot->addVerbInteraction(Verb::anyVerb, Event([index](EventInfo e) {
	//	SDL_Log("Selected item %d", index);
	//	GameObject* item = e.player.getInventory().getItemAt(index);
	//	item->interact(e.clickPos, e.player.getVerb());
	//	//e.player.setSelectedObject(item);

	//}));
	//itemSlot->setMouseOverEvent(Event([this, index](EventInfo e) {
	//	e.player.setTargetObject(e.player.getInventory().getItemAt(index));
	//	updateUI();
	//}));
	//itemSlot->setMouseNotOverEvent(Event([this, index](EventInfo e) {
	//	e.player.setTargetObject(nullptr);
	//	updateUI();
	//}));

	//Game& game = Game::getInstance();
	addUIElement(*itemSlot);
	itemSlots.push_back(itemSlot);

	return *itemSlot;
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

void UILayout::addUIElement(GameObject& uiElement) {
	uiElements.push_back(&uiElement);
}

void UILayout::removeUIElement(GameObject& uiElement) {
	std::vector<GameObject*>::iterator position = std::find(uiElements.begin(), uiElements.end(), &uiElement);
	if (position != uiElements.end()) // == myVector.end() means the element was not found
		uiElements.erase(position);
}

void UILayout::toggleVisibility() {
	visible = !visible;
}

void UILayout::setHighlightedVerb(Verb* verb) {
	highlightedVerb = verb;
}

GameObject* UILayout::getUIElement(std::string name) {
	for (GameObject* element : uiElements) {
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