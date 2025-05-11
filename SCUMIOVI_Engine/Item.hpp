#pragma once

#include "GameObject.hpp"

class Item : public GameObject {
public:
	Item(std::string name,
		Vector2<> position,
		Texture* sceneTexture,
		Texture* inventoryTexture
	);
};