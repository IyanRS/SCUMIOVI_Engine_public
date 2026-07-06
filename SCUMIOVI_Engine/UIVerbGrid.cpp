#include "UIVerbGrid.hpp"
#include "UILayout.hpp"
#include "Game.hpp"

UIVerbGrid::UIVerbGrid(std::string name, Vector2<> position)
	: UIElement(name, position, Vector2<>(0, 0)) {
}

void UIVerbGrid::addButton(std::shared_ptr<UIVerbButton> button) {
	addChild(button);
	buttons.push_back(button);
}

std::vector<std::shared_ptr<UIVerbButton>>& UIVerbGrid::getButtons() {
	return buttons;
}

void UIVerbGrid::refresh() {
	UIElement::refresh();

	// The active layout tracks which verb (if any) should be highlighted,
	// e.g. because the player is hovering an object with a matching default
	// verb. Mirrors the "ask the current UI" idiom already used by UIItemSlot.
	auto* layout = Game::getInstance().getUI().getLayout();
	std::shared_ptr<Verb> highlighted = layout ? layout->getHighlightedVerb() : nullptr;

	for (auto& button : buttons) {
		if (highlighted && button->getVerb() == highlighted)
			button->highlight();
		else
			button->stopHighlight();
	}
}
