#pragma once

#include <vector>

#include "GameObject.hpp"
#include "Verb.hpp"

class Inventory {
	std::vector<GameObject*> items;
public:
	void addItem(GameObject* item);
	void removeItem(GameObject& item);
	GameObject& getItemAt(int index);
	std::vector<GameObject*>& getItems();
};