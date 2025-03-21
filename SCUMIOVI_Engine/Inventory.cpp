#include "Inventory.hpp"

void Inventory::addItem(GameObject* item) {
	items.push_back(item);
}

void Inventory::removeItem(GameObject& item) {
	auto it = std::find(items.begin(), items.end(), &item);
	if (it == items.end())
		return;
	items.erase(it);
}

GameObject* Inventory::getItemAt(int index) {
	if (index < 0 || index >= items.size()) {
		return nullptr;
	}
	return items[index];
}

std::vector<GameObject*>& Inventory::getItems() {
	return items;
}