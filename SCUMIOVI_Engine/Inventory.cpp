#include "Inventory.hpp"

void Inventory::addItem(std::shared_ptr<GameObject> item) {
	items.push_back(item);
}

void Inventory::removeItem(GameObject& item) {
	auto it = std::find_if(items.begin(), items.end(), [&item](const std::shared_ptr<GameObject>& ptr) {
		return ptr.get() == &item;
	});
	if (it == items.end())
		return;
	items.erase(it);
}

std::shared_ptr<GameObject> Inventory::getItemAt(int index) {
	if (index < 0 || index >= items.size()) {
		return nullptr;
	}
	return items[index];
}

std::vector<std::shared_ptr<GameObject>>& Inventory::getItems() {
	return items;
}