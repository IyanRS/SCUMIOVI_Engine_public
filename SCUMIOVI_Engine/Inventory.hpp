#pragma once

#include <vector>

#include "GameObject.hpp"
#include "Verb.hpp"

class Inventory {
	std::vector<std::shared_ptr<GameObject>> items;
public:
	void addItem(std::shared_ptr<GameObject> item);
	void removeItem(std::shared_ptr<GameObject> item);
	std::shared_ptr<GameObject> getItemAt(int index);
	std::vector<std::shared_ptr<GameObject>>& getItems();
};