#include "Inventory.hpp"
#include <algorithm>

void Inventory::addItem(std::shared_ptr<GameObject> item) {
	items.push_back(item);
}

void Inventory::removeItem(std::shared_ptr<GameObject> item) {
	auto it = std::find(items.begin(), items.end(), item);
	if (it != items.end()) {
		items.erase(it);
	}
}

std::shared_ptr<GameObject> Inventory::getItemAt(int index) {
	if (index < 0 || index >= (int)items.size()) {
		return nullptr;
	}
	return items[index];
}

std::vector<std::shared_ptr<GameObject>>& Inventory::getItems() {
	return items;
}