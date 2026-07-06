#pragma once

#include "UIElement.hpp"
#include "Text.hpp"

// A label that keeps itself in sync with the current player's active verb
// sentence (e.g. "Use key on door"), refreshed automatically by UILayout's
// generic refresh pass instead of being looked up by a magic element name.
class UIInfoLabel : public UIElement {
public:
	UIInfoLabel(std::string name, Vector2<> position, Text format);
	void refresh() override;
};
