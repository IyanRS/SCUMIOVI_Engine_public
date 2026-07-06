#include "UI.hpp"
#include <SDL3/SDL.h>
#include <stdexcept>

std::vector<std::shared_ptr<GameObject>>& UI::getUIElements() {
	static std::vector<std::shared_ptr<GameObject>> empty;
	if (!currentLayout) return empty;
	return currentLayout->getUIElements();
}

std::vector<std::shared_ptr<GameObject>> UI::getRenderElements() {
	std::vector<std::shared_ptr<GameObject>> out;
	if (currentLayout) {
		auto base = currentLayout->getRenderElements();
		out.insert(out.end(), base.begin(), base.end());
	}
	if (overlayLayout) {
		auto overlay = overlayLayout->getRenderElements();
		out.insert(out.end(), overlay.begin(), overlay.end());
	}
	return out;
}

void UI::updateUI() {
	if (currentLayout) currentLayout->updateUI();
	if (overlayLayout) overlayLayout->updateUI();
}

void UI::openOverlay(std::string name, bool pauseGame) {
	for (auto& layout : layouts) {
		if (layout->getName() == name) {
			overlayLayout = layout.get();
			overlayPausesGame = pauseGame;
			return;
		}
	}
	SDL_Log("UI::openOverlay: no layout named '%s'", name.c_str());
}

void UI::closeOverlay() {
	overlayLayout = nullptr;
	overlayPausesGame = false;
}

UILayout* UI::getOverlay() {
	return overlayLayout;
}

bool UI::isGamePaused() {
	return overlayLayout != nullptr && overlayPausesGame;
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
	// Iterate all layouts — finds the LAST match (most recently created).
	// Without this full scan, duplicate-named layouts from prior test sections
	// (Game is a singleton across Catch2 sections) would leave a stale layout active.
	for (auto& layout : layouts) {
		if (layout->getName() == name) {
			currentLayout = layout.get();
		}
	}
}

UILayout* UI::getLayout() {
	return currentLayout;
}

void UI::createLayout(std::string name) {
	layouts.push_back(std::make_unique<UILayout>(name));
}

UILayout& UI::getLayout(std::string name) {
	for (auto& layout : layouts) {
		if (layout->getName() == name) {
			return *layout;
		}
	}
	throw std::runtime_error("UI::getLayout: no layout named '" + name + "'");
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