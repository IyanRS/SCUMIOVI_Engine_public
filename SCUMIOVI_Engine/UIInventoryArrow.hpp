#pragma once

#include "GameObject.hpp"

class UIInventoryArrow : public GameObject {
	int increment;
public:
	UIInventoryArrow(Vector2<> position, std::shared_ptr<Texture> texture, int increment);
};