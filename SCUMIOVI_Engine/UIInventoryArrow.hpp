#pragma once

#include "GameObject.hpp"

class UIInventoryArrow : public GameObject {
	int increment;
public:
	UIInventoryArrow(Vector2<> position, Texture* texture, int increment);
};