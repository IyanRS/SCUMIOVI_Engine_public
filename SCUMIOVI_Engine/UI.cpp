#include "UI.hpp"

std::vector<GameObject*>& UI::getUIElements() {
	return currentLayout->getUIElements();
}

void UI::updateUI() {
	currentLayout->updateUI();
}

void UI::highlightVerb(Verb& verb) {
	currentLayout->highlightVerb(verb);
}

void UI::removeHightlightVerb() {
	currentLayout->removeHightlightVerb();
}

void UI::toggleVisibility() {
	currentLayout->toggleVisibility();
}

GameObject* UI::getUIElement(std::string name) {
	return currentLayout->getUIElement(name);
}

void UI::setLayout(std::string name) {
	for (UILayout* layout : layouts) {
		if (layout->getName() == name) {
			currentLayout = layout;
		}
	}
}

void UI::createLayout(std::string name) {
	UILayout* layout = new UILayout(name); // TODO: free memory on destroy
	layouts.push_back(layout);
}

UILayout& UI::getLayout(std::string name) {
	for (UILayout* layout : layouts) {
		if (layout->getName() == name) {
			return *layout;
		}
	}
	// TODO: throw error
}

void UI::createDialogueOptions(DialogueNode& dialogueNode) {
	currentLayout->createDialogueOptions(dialogueNode);
}

void UI::clearDialogueOptions() {
	currentLayout->clearDialogueOptions();
}

//#include "UI.hpp"
//#include "Game.hpp"
//
//std::vector<GameObject*>& UI::getUIElements() {
//	return uiElements;
//}
//
//void UI::updateUI() {
//	Game& game = Game::getInstance();
//	Verb* currentVerb = game.getCurrentPlayer().getVerb();
//	if (currentVerb != nullptr) {
//		// TODO: uiCurrentVerbInfo reference in UI class
//		getUIElement("uiCurrentVerbInfo")->setTextOver(currentVerb->getDisplayName());
//	}
//}
//
//void UI::removeHightlightVerb() {
//	Game& game = Game::getInstance();
//	for (UIVerbButton* button : uiVerbButtons) {
//		button->setTextOver(button->getTextOver().toString());
//		//if (uiElement.getName() == verb.getName() + "Button") {
//		//	auto buttonText = uiElement.getTextOver();
//		//	int highlightedColor[4] = { 230, 44, 230, 0xff };
//		//	Text highlightedText = Text(buttonText.toString(), buttonText.getSize(), highlightedColor, buttonText.hasOutline(), buttonText.getTextOverMode(), buttonText.hasShadow());
//		//	uiElement.setTextOver(highlightedText);
//		//}
//		//else {
//		//	uiElement.setTextOver(uiElement.getTextOver().toString());
//		//}
//	}
//}
//
//void UI::highlightVerb(Verb& verb) {
//	Game& game = Game::getInstance();
//	removeHightlightVerb();
//	for (UIVerbButton* button : uiVerbButtons) {
//		if (button->getVerb() == verb) {
//			button->highlight();
//		}
//	}
//}
//
////GameObject& UI::createUIVerbButton(Verb& verb, Vector2<> position, Vector2<> size, Texture* verbBackground, Event& onUIUpdate) {
////	GameObject* uiVerbText = new GameObject(verb.getName() + "Button", position, size, verbBackground);
////	int color[4] = { 125, 44, 125, 0xff };
////	Text text = Text(verb.getDisplayName(), 11, color, false, TEXT_CENTERED, true);
////	uiVerbText->setTextOver(text);
////	uiVerbText->setTextFormat(text);
////	uiVerbText->setDefaultVerb(&Verb::anyVerb);
////	std::string verbName = verb.getName();
////	uiVerbText->addVerbInteraction(Verb::anyVerb, Event([verbName] {
////		Game& game = Game::getInstance();
////		Player& player = game.getCurrentPlayer();
////		player.setVerb(verbName);
////		}) + onUIUpdate);
////	uiVerbText->setMouseOverEvent(Event([this, verbName] {
////		//TODO:
////		//highlightUIVerb(verbName);
////		}));
////	uiVerbText->setMouseNotOverEvent(Event([this, verbName](EventInfo e) {
////		auto button = e.game.getUI().getUIElement(verbName + "Button");
////		button->setTextOver(button->getTextOver().toString());
////		}));
////	Game& game = Game::getInstance();
////
////	game.getUI().addUIElement(*uiVerbText);
////
////	return *uiVerbText;
////}
//
//std::vector<UIVerbButton*> UI::createVerbsMatrix(Vector2<> position, std::vector<std::vector<Verb>> verbs, Texture* verbBackground) {
//	int height = 14;
//	std::vector<int> columnLengths;
//
//	std::vector<UIVerbButton*> buttons;
//
//	int columns = verbs[0].size();
//	int rows = verbs.size();
//
//	for (int i = 0; i < columns; i++) {
//		int maxLengthColumn = 0;
//		for (int j = 0; j < rows; j++) {
//			if (verbs[j][i].getDisplayName().length() > maxLengthColumn) {
//				maxLengthColumn = verbs[j][i].getDisplayName().length();
//			}
//		}
//		columnLengths.push_back(maxLengthColumn);
//	}
//	for (int i = 0; i < columns; i++) {
//		for (int j = 0; j < rows; j++) {
//			Vector2<> buttonPos = Vector2<>(position.x, position.y + (height + 2) * j);
//			for (int k = 0; k < i; k++) {
//				buttonPos.x += columnLengths[k] * 7;
//				buttonPos.x += 2;
//			}
//			UIVerbButton* button = new UIVerbButton(verbs[j][i], buttonPos, Vector2<>(columnLengths[i] * 7, height), verbBackground);
//			addUIElement(*button);
//			uiVerbButtons.push_back(button);
//			buttons.push_back(button);
//		}
//	}
//	return buttons;
//}
//
//GameObject& UI::createUIItemSlot(Vector2<> position, Vector2<> size, Texture* background, Event& onUIUpdate) {
//	GameObject* itemSlot = new GameObject("itemSlot" + position.toString(), position, size, background);
//
//	Game& game = Game::getInstance();
//	game.getUI().addUIElement(*itemSlot);
//
//	return *itemSlot;
//}
//
//void UI::createInventoryMatrix(Vector2<> position, int columns, int rows, Texture* background, Event& onUIUpdate) {
//	int width = 40;
//	int height = 22;
//
//	for (int i = 0; i < columns; i++) {
//		for (int j = 0; j < rows; j++) {
//			Vector2<> slotPos = Vector2<>(position.x + (width + 2) * i, position.y + (height + 2) * j);
//			createUIItemSlot(slotPos, Vector2<>(width, height), background, onUIUpdate);
//		}
//	}
//}
//
//void UI::addUIElement(GameObject& uiElement) {
//	uiElements.push_back(&uiElement);
//}
//
//void UI::toggleVisibility() {
//	visible = !visible;
//}
//
//GameObject* UI::getUIElement(std::string name) {
//	for (GameObject* element : uiElements) {
//		if (element->getName() == name) {
//			return element;
//		}
//	}
//	return nullptr;
//}