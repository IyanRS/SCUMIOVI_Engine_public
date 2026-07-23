#include "UIVerbGrid.hpp"
#include "UILayout.hpp"
#include "Game.hpp"

// Geometry shared with the original createVerbsMatrix layout: an assumed
// character advance, the button height, and the gap between rows/columns.
namespace {
	constexpr int BUTTON_CHAR_WIDTH = 7;
	constexpr int BUTTON_HEIGHT = 14;
	constexpr int BUTTON_GAP = 2;
}

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

void UIVerbGrid::setGridShape(int newRows, int newColumns) {
	rows = newRows;
	columns = newColumns;
	layoutButtons();
}

void UIVerbGrid::layoutButtons() {
	if (rows <= 0 || columns <= 0 || (int)buttons.size() < rows * columns)
		return;

	// Column width = the longest (localized) label in that column.
	std::vector<int> columnLengths(columns, 0);
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			int length = (int)buttons[i * rows + j]->getVerb()->getDisplayName().length();
			if (length > columnLengths[i])
				columnLengths[i] = length;
		}
	}

	float x = 0.0f;
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			auto& button = buttons[i * rows + j];
			button->setOffset(Vector2<>(x, (float)((BUTTON_HEIGHT + BUTTON_GAP) * j)));
			button->setSize(Vector2<>((float)(columnLengths[i] * BUTTON_CHAR_WIDTH), (float)BUTTON_HEIGHT));
		}
		x += (float)(columnLengths[i] * BUTTON_CHAR_WIDTH + BUTTON_GAP);
	}
}

void UIVerbGrid::refresh() {
	UIElement::refresh();

	// The buttons re-resolved their labels above (UIVerbButton::refresh runs
	// tr()); re-derive the column geometry from what they now say, so a
	// locale switch resizes the grid to fit the new labels.
	layoutButtons();

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
