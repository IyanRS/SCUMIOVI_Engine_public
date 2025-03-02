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

void UI::setHighlightedVerb(Verb* verb) {
	currentLayout->setHighlightedVerb(verb);
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

void UI::incrementInventoryIndex(int increment) {
	currentLayout->incrementInventoryIndex(increment);
}

int UI::getInventoryIndex() {
	return currentLayout->getInventoryIndex();
}