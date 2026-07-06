#pragma once

#include "UIElement.hpp"

class UIItemSlot : public UIElement {
public:
	UIItemSlot(Vector2<> position, Vector2<> size, std::shared_ptr<Texture> slotBackground, int relativeIndex);
};