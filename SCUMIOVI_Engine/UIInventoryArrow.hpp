#pragma once

#include "UIElement.hpp"

class UIInventoryArrow : public UIElement {
	int increment;
public:
	UIInventoryArrow(Vector2<> position, std::shared_ptr<Texture> texture, int increment);
};