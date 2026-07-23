#pragma once

#include "GameObject.hpp"
#include <memory>

class Item : public GameObject {
	std::shared_ptr<Texture> inventoryTexture;
public:
	Item(std::string name,
		Vector2<> position,
		std::shared_ptr<Texture> sceneTexture,
		std::shared_ptr<Texture> inventoryTexture
	);

	// Builds the GameObject that represents this item inside an inventory
	// (inventory texture, "item" tag, this item's display name, and the
	// give-verb select/deselect behaviour). Used on pickup, and by the save
	// system to rebuild inventories on load.
	std::shared_ptr<GameObject> createInventoryObject();
};
