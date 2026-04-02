#pragma once

#include "GameObject.hpp"

class UIItemSlot : public GameObject {
public:
	UIItemSlot(Vector2<> position, Vector2<> size, std::shared_ptr<Texture> slotBackground, int relativeIndex);
};