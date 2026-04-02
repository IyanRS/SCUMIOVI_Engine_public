#include "UI.hpp"

std::vector<std::shared_ptr<GameObject>>& UI::getUIElements() {
	static std::vector<std::shared_ptr<GameObject>> empty;
	if (!currentLayout) return empty;
	return currentLayout->getUIElements();
}

void UI::updateUI() {
	if (!currentLayout) return;
	currentLayout->updateUI();
}

void UI::highlightVerb(std::shared_ptr<Verb> verb) {
	if (!currentLayout) return;
	currentLayout->highlightVerb(verb);
}

void UI::removeHightlightVerb() {
	if (!currentLayout) return;
	currentLayout->removeHightlightVerb();
}

void UI::setHighlightedVerb(std::shared_ptr<Verb> verb) {
	if (!currentLayout) return;
	currentLayout->setHighlightedVerb(verb);
}

void UI::toggleVisibility() {
	if (!currentLayout) return;
	currentLayout->toggleVisibility();
}

std::shared_ptr<GameObject> UI::getUIElement(std::string name) {
	if (!currentLayout) return nullptr;
	return currentLayout->getUIElement(name);
}

void UI::setLayout(std::string name) {
	for (UILayout* layout : layouts) {
		if (layout->getName() == name) {
			currentLayout = layout;
		}
	}
}

UILayout* UI::getLayout() {
	return currentLayout;
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
	if (!currentLayout) return;
	currentLayout->createDialogueOptions(dialogueNode);
}

void UI::clearDialogueOptions() {
	if (!currentLayout) return;
	currentLayout->clearDialogueOptions();
}

void UI::incrementInventoryIndex(int increment) {
	if (!currentLayout) return;
	currentLayout->incrementInventoryIndex(increment);
}

int UI::getInventoryIndex() {
	if (!currentLayout) return 0;
	return currentLayout->getInventoryIndex();
}

void UI::setDialogueLayout(UILayout* layout) {
	dialogueLayout = layout;
}
UILayout* UI::getDialogueLayout() {
	return dialogueLayout;
}

void UI::setAfterDialogueLayout(UILayout* layout) {
	afterDialogueLayout = layout;
}
UILayout* UI::getAfterDialogueLayout() {
	return afterDialogueLayout;
}