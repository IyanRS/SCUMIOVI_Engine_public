#pragma once

#include "GameObject.hpp"
#include <memory>

class Item : public GameObject {
public:
	Item(std::string name,
		Vector2<> position,
		std::shared_ptr<Texture> sceneTexture,
		std::shared_ptr<Texture> inventoryTexture
	);
};