#pragma once

#include "UIElement.hpp"
#include "UIVerbButton.hpp"

#include <vector>

// Container for a set of verb buttons. Owns them as children (so they move,
// render and hit-test as part of the tree) and applies the layout's currently
// highlighted verb to the matching button each refresh, replacing the manual
// bookkeeping that used to live in UILayout.
class UIVerbGrid : public UIElement {
	std::vector<std::shared_ptr<UIVerbButton>> buttons;
	// Grid shape, set by setGridShape once every button has been added.
	// Buttons are stored column-major: buttons[column * rows + row].
	int rows = 0;
	int columns = 0;
	// Measures the buttons' current (localized) labels and re-derives column
	// widths and offsets from them — run on every refresh, so a locale
	// switch resizes the grid along with the retitled buttons.
	void layoutButtons();
public:
	UIVerbGrid(std::string name, Vector2<> position);
	void addButton(std::shared_ptr<UIVerbButton> button);
	std::vector<std::shared_ptr<UIVerbButton>>& getButtons();
	// Declares the grid's shape and performs the initial layout. Until this
	// is called, buttons keep whatever position/size they were built with.
	void setGridShape(int rows, int columns);
	void refresh() override;
};
