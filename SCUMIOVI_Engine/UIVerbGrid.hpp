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
public:
	UIVerbGrid(std::string name, Vector2<> position);
	void addButton(std::shared_ptr<UIVerbButton> button);
	std::vector<std::shared_ptr<UIVerbButton>>& getButtons();
	void refresh() override;
};
